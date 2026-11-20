/** Copyright © 2019 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3.0 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */
/*
 * TileManager.cpp
 *
 *  Created on: Feb 23, 2018
 *      Author: mschefer
 */

#include "ElementsKernel/Exception.h"
#include "SEFramework/Image/TileManager.h"

namespace SourceXtractor {

static std::shared_ptr<TileManager> s_instance;
static Elements::Logging s_tile_logger = Elements::Logging::getLogger("TileManager");

bool TileKey::operator==(const TileKey& other) const {
  return m_source == other.m_source && m_tile_x == other.m_tile_x && m_tile_y == other.m_tile_y;
}

std::string TileKey::getRepr() const {
  std::ostringstream str;
  str << m_source.get() << "[" << m_source->getRepr() << "] " << m_tile_x << "," << m_tile_y;
  return str.str();
}


TileManager::WritePin& TileManager::WritePin::operator=(WritePin&& other) noexcept {
  if (this != &other) {
    reset();
    m_tile = std::move(other.m_tile);
  }
  return *this;
}

TileManager::WritePin::~WritePin() {
  reset();
}

void TileManager::WritePin::reset() noexcept {
  if (m_tile) {
    // Publish the pixel and dirty flag writes before eviction or another writer can use the tile.
    m_tile->m_write_pinned.store(false, std::memory_order_release);
    m_tile.reset();
  }
}

TileManager::WritePin::WritePin(std::shared_ptr<ImageTile> tile) : m_tile(std::move(tile)) {
  bool expected = false;
  if (!m_tile->m_write_pinned.compare_exchange_strong(expected, true, std::memory_order_acquire)) {
    throw Elements::Exception() << "Image tile is already pinned for writing";
  }
}

TileManager::TileManager() : m_tile_width(256), m_tile_height(256),
                             m_max_memory(100 * 1024L * 1024L), m_total_memory_used(0) {
}

TileManager::~TileManager() {
  try {
    saveAllTiles();
  } catch (const std::exception& e) {
    s_tile_logger.error() << "Error while saving tiles at destruction: " << e.what();
  }
}

void TileManager::setOptions(int tile_width, int tile_height, int max_memory) {
  flush();

  boost::lock_guard<boost::shared_mutex> wr_lock(m_mutex);
  m_tile_width = tile_width;
  m_tile_height = tile_height;
  m_max_memory = max_memory * 1024L * 1024L;
}

void TileManager::flush() {
  boost::lock_guard<boost::shared_mutex> wr_lock(m_mutex);
  saveAllTilesLocked();
  m_tile_list.clear();
  m_tile_map.clear();
  m_total_memory_used = 0;
}

/*
 * boost::upgrade_lock can only be acquired by a single thread, even if none of them
 * ends needing an exclusive lock. Cache lookup must be done with a shared_lock instead
 * so multiples can retrieve from the cache at the same time.
 */
std::shared_ptr<ImageTile> TileManager::tryTileFromCache(const TileKey& key, WritePin* write_pin) {
  boost::shared_lock<boost::shared_mutex> shared_rd_lock(m_mutex);

  auto it = m_tile_map.find(key);
  if (it != m_tile_map.end()) {
#ifndef NDEBUG
    s_tile_logger.debug() << "Cache hit " << key;
#endif
    if (write_pin) {
      *write_pin = WritePin(it->second);
    }
    return it->second;
  }
  return nullptr;
}

/*
 * If the mutex does not exist, we need an upgradable lock
 */
std::shared_ptr<boost::mutex>& TileManager::getMutexForImageSource(const ImageSource *src_ptr) {
  boost::upgrade_lock<boost::shared_mutex> upgrade_lock(m_mutex);
  auto mit = m_mutex_map.find(src_ptr);
  if (mit == m_mutex_map.end()) {
    boost::upgrade_to_unique_lock<boost::shared_mutex> unique_lock(upgrade_lock);
    mit = m_mutex_map.emplace(src_ptr, std::make_shared<boost::mutex>()).first;
  }
  return mit->second;
}

std::shared_ptr<ImageTile> TileManager::getTileForPixel(int x, int y,
                                                        std::shared_ptr<const ImageSource> source) {
  return getTileForPixel(x, y, std::move(source), nullptr);
}

TileManager::WritePin TileManager::pinTileForPixel(int x, int y, std::shared_ptr<const ImageSource> source) {
  WritePin pin;
  getTileForPixel(x, y, std::move(source), &pin);
  return pin;
}

std::shared_ptr<ImageTile> TileManager::getTileForPixel(int x, int y, std::shared_ptr<const ImageSource> source,
                                                        WritePin* write_pin) {
  x = x / m_tile_width * m_tile_width;
  y = y / m_tile_height * m_tile_height;
  TileKey key{std::static_pointer_cast<const ImageSource>(source), x, y};

  // Try from the cache, this can be done by multiple threads in parallel
  auto tile = tryTileFromCache(key, write_pin);
  if (tile) {
    return tile;
  }

  // Cache miss, we need to ask the underlying source.
  // First, we need a mutex only for that source, and that needs writing to the tile manager.
  auto img_mutex = getMutexForImageSource(source.get());

  // Here we block access only to this specific image source
  boost::lock_guard<boost::mutex> img_lock(*img_mutex);

  // Try again from the cache, maybe someone put it there while we waited for the image lock
  tile = tryTileFromCache(key, write_pin);
  if (tile) {
    return tile;
  }

  tile = source->getImageTile(x, y,
                              std::min(m_tile_width, source->getWidth() - x),
                              std::min(m_tile_height, source->getHeight() - y));

  // Here we need to acquire the mutex in write mode!
  boost::lock_guard<boost::shared_mutex> wr_lock(m_mutex);
  addTile(key, tile);
  auto pin = write_pin ? WritePin(tile) : WritePin{};
  removeExtraTiles();
  if (write_pin) {
    *write_pin = std::move(pin);
  }
  return tile;
}

std::shared_ptr<TileManager> TileManager::getInstance() {
  if (s_instance == nullptr) {
    s_instance = std::make_shared<TileManager>();
  }
  return s_instance;
}

void TileManager::saveAllTiles() {
  boost::lock_guard<boost::shared_mutex> wr_lock(m_mutex);

  saveAllTilesLocked();
}

void TileManager::saveAllTilesLocked() {
  for (const auto& entry : m_tile_map) {
    if (entry.second->m_write_pinned.load(std::memory_order_acquire)) {
      throw Elements::Exception() << "Cannot save the tile cache while a tile is pinned for writing";
    }
  }
  for (const auto& tile_key : m_tile_list) {
    m_tile_map.at(tile_key)->saveIfModified();
  }
}

int TileManager::getTileWidth() const {
  return m_tile_width;
}

int TileManager::getTileHeight() const {
  return m_tile_height;
}

void TileManager::removeTile(TileKey tile_key) {
#ifndef NDEBUG
  s_tile_logger.debug() << "Cache eviction " << tile_key;
#endif

  auto& tile = m_tile_map.at(tile_key);

  tile->saveIfModified();
  m_total_memory_used -= tile->getTileMemorySize();

  m_tile_map.erase(tile_key);
}

void TileManager::removeExtraTiles() {
  // Visit each candidate once. Pinned tiles can temporarily put the cache over budget.
  auto it = m_tile_list.end();
  while (m_total_memory_used > m_max_memory && it != m_tile_list.begin()) {
    --it;
    if (!m_tile_map.at(*it)->m_write_pinned.load(std::memory_order_acquire)) {
      removeTile(*it);
      it = m_tile_list.erase(it);
    }
  }
}

void TileManager::addTile(TileKey key, std::shared_ptr<ImageTile> tile) {
#ifndef NDEBUG
  s_tile_logger.debug() << "Cache miss " << key;
#endif

  m_tile_list.push_front(key);
  try {
    m_tile_map.emplace(key, tile);
  } catch (...) {
    m_tile_list.pop_front();
    throw;
  }
  m_total_memory_used += tile->getTileMemorySize();
}

}

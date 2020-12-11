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


TileManager::TileManager() : m_tile_width(256), m_tile_height(256),
                             m_max_memory(100 * 1024L * 1024L), m_total_memory_used(0) {
}

TileManager::~TileManager() {
  saveAllTiles();
}

void TileManager::setOptions(int tile_width, int tile_height, int max_memory) {
  std::lock_guard<std::recursive_mutex> lock(m_mutex);

  flush();

  m_tile_width = tile_width;
  m_tile_height = tile_height;
  m_max_memory = max_memory * 1024L * 1024L;
}

void TileManager::flush() {
  std::lock_guard<std::recursive_mutex> lock(m_mutex);

  // empty anything still stored in cache
  saveAllTiles();
  m_tile_list.clear();
  m_tile_map.clear();
  m_total_memory_used = 0;
}

std::shared_ptr<ImageTile> TileManager::getTileForPixel(int x, int y,
                                                        std::shared_ptr<const ImageSource> source) {
  std::lock_guard<std::recursive_mutex> lock(m_mutex);

  x = x / m_tile_width * m_tile_width;
  y = y / m_tile_height * m_tile_height;

  TileKey key{std::static_pointer_cast<const ImageSource>(source), x, y};
  auto it = m_tile_map.find(key);
  if (it != m_tile_map.end()) {
#ifndef NDEBUG
    s_tile_logger.debug() << "Cache hit " << key;
#endif
    return std::dynamic_pointer_cast<ImageTile>(it->second);
  }
  else {
    auto tile = source->getImageTile(x, y,
                                     std::min(m_tile_width, source->getWidth() - x),
                                     std::min(m_tile_height, source->getHeight() - y));
    addTile(key, std::static_pointer_cast<ImageTile>(tile));
    removeExtraTiles();
    return tile;
  }
}

std::shared_ptr<TileManager> TileManager::getInstance() {
  if (s_instance == nullptr) {
    s_instance = std::make_shared<TileManager>();
  }
  return s_instance;
}

void TileManager::saveAllTiles() {
  std::lock_guard<std::recursive_mutex> lock(m_mutex);

  for (auto tile_key : m_tile_list) {
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
  while (m_total_memory_used > m_max_memory) {
    assert(m_tile_list.size() > 0);
    auto tile_to_remove = m_tile_list.back();
    removeTile(tile_to_remove);
    m_tile_list.pop_back();
  }
}

void TileManager::addTile(TileKey key, std::shared_ptr<ImageTile> tile) {
#ifndef NDEBUG
  s_tile_logger.debug() << "Cache miss " << key;
#endif

  m_tile_map[key] = tile;
  m_tile_list.push_front(key);
  m_total_memory_used += tile->getTileMemorySize();
}

}

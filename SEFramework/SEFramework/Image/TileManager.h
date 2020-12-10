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
 * TileManager.h
 *
 *  Created on: Feb 23, 2018
 *      Author: mschefer
 */

#ifndef _SEFRAMEWORK_IMAGE_TILEMANAGER_H_
#define _SEFRAMEWORK_IMAGE_TILEMANAGER_H_

#include <iostream>
#include <thread>
#include <mutex>

#include <list>
#include <unordered_map>

#include <ElementsKernel/Logging.h>

#include "SEFramework/Image/ImageTile.h"
#include "SEFramework/Image/ImageSource.h"

namespace SourceXtractor {


struct TileKey {
  std::shared_ptr<const ImageSource> m_source;
  int m_tile_x, m_tile_y;

  bool operator==(const TileKey& other) const {
    return m_source == other.m_source && m_tile_x == other.m_tile_x && m_tile_y == other.m_tile_y;
  }

  std::string getRepr() const {
    std::ostringstream str;
    str << m_source.get() << "[" << m_source->getRepr() << "] " << m_tile_x << ","  << m_tile_y;
    return str.str();
  }
};

inline std::ostream& operator << (std::ostream &out, const TileKey &tk) {
  out << tk.getRepr();
  return out;
}

}

namespace std {

template <>
struct hash<SourceXtractor::TileKey>
{
  std::size_t operator()(const SourceXtractor::TileKey& key) const {
    std::size_t hash = 0;
    boost::hash_combine(hash, key.m_source);
    boost::hash_combine(hash, key.m_tile_x);
    boost::hash_combine(hash, key.m_tile_y);
    return hash;
  }
};

}

namespace SourceXtractor {

class TileManager {
public:

  TileManager() : m_tile_width(256), m_tile_height(256), m_max_memory(100 * 1024L * 1024L), m_total_memory_used(0),
                  m_tile_logger(Elements::Logging::getLogger("TileManager")) {
  }

  virtual ~TileManager() {
    saveAllTiles();
  }

  // Actually not thread safe, call before starting the multi-threading
  void setOptions(int tile_width, int tile_height, int max_memory) {
    std::lock_guard<std::recursive_mutex> lock(m_mutex);

    flush();

    m_tile_width = tile_width;
    m_tile_height = tile_height;
    m_max_memory = max_memory*1024L*1024L;
  }

  void flush() {
    std::lock_guard<std::recursive_mutex> lock(m_mutex);

    // empty anything still stored in cache
    saveAllTiles();
    m_tile_list.clear();
    m_tile_map.clear();
    m_total_memory_used = 0;
  }

  std::shared_ptr<ImageTile> getTileForPixel(int x, int y, std::shared_ptr<const ImageSource> source) {
    std::lock_guard<std::recursive_mutex> lock(m_mutex);

    x = x / m_tile_width * m_tile_width;
    y = y / m_tile_height * m_tile_height;

    TileKey key {std::static_pointer_cast<const ImageSource>(source), x, y};
    auto it = m_tile_map.find(key);
    if (it != m_tile_map.end()) {
#ifndef NDEBUG
      m_tile_logger.debug() << "Cache hit " << key;
#endif
      return std::dynamic_pointer_cast<ImageTile>(it->second);
    } else {
      auto tile = source->getImageTile(x, y,
          std::min(m_tile_width, source->getWidth()-x), std::min(m_tile_height, source->getHeight()-y));
      addTile(key, std::static_pointer_cast<ImageTile>(tile));
      removeExtraTiles();
      return tile;
    }
  }

  static std::shared_ptr<TileManager> getInstance() {
    if (s_instance == nullptr) {
      s_instance = std::make_shared<TileManager>();
    }
    return s_instance;
  }

  void saveAllTiles() {
    std::lock_guard<std::recursive_mutex> lock(m_mutex);

    for (auto tile_key : m_tile_list) {
      m_tile_map.at(tile_key)->saveIfModified();
    }
  }

  int getTileWidth() const {
    return m_tile_width;
  }

  int getTileHeight() const {
    return m_tile_height;
  }

private:

  void removeTile(TileKey tile_key) {
#ifndef NDEBUG
    m_tile_logger.debug() << "Cache eviction " << tile_key;
#endif

    auto& tile = m_tile_map.at(tile_key);

    tile->saveIfModified();
    m_total_memory_used -= tile->getTileMemorySize();

    m_tile_map.erase(tile_key);
  }

  void removeExtraTiles() {
    while (m_total_memory_used > m_max_memory) {
      assert(m_tile_list.size() > 0);
      auto tile_to_remove = m_tile_list.back();
      removeTile(tile_to_remove);
      m_tile_list.pop_back();
    }
  }

  void addTile(TileKey key, std::shared_ptr<ImageTile> tile) {
#ifndef NDEBUG
    m_tile_logger.debug() << "Cache miss " << key;
#endif

    m_tile_map[key] = tile;
    m_tile_list.push_front(key);
    m_total_memory_used += tile->getTileMemorySize();
  }

  int m_tile_width, m_tile_height;
  long m_max_memory;
  long m_total_memory_used;

  std::unordered_map<TileKey, std::shared_ptr<ImageTile>> m_tile_map;
  std::list<TileKey> m_tile_list;

  std::recursive_mutex m_mutex;

  Elements::Logging m_tile_logger;

  static std::shared_ptr<TileManager> s_instance;
};

}


#endif /* _SEFRAMEWORK_IMAGE_TILEMANAGER_H_ */

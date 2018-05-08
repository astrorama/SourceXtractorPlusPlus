/*
 * TileManager.h
 *
 *  Created on: Feb 23, 2018
 *      Author: mschefer
 */

#ifndef _SEFRAMEWORK_IMAGE_TILEMANAGER_H_
#define _SEFRAMEWORK_IMAGE_TILEMANAGER_H_

#include <iostream>

#include <list>
#include <unordered_map>

#include "SEFramework/Image/ImageTile.h"
#include "SEFramework/Image/ImageSource.h"

namespace SExtractor {

struct TileKey {
  std::shared_ptr<const ImageSourceBase> m_source;
  int m_tile_x, m_tile_y;

  bool operator==(const TileKey& other) const {
    return m_source == other.m_source && m_tile_x == other.m_tile_x && m_tile_y == other.m_tile_y;
  }
};

}

namespace std {

template <>
struct hash<SExtractor::TileKey>
{
  std::size_t operator()(const SExtractor::TileKey& key) const {
    std::size_t hash = 0;
    boost::hash_combine(hash, key.m_source);
    boost::hash_combine(hash, key.m_tile_x);
    boost::hash_combine(hash, key.m_tile_y);
    return hash;
  }
};

}

namespace SExtractor {

class TileManager {
public:

  TileManager() : m_tile_width(256), m_tile_height(256), m_max_memory(100*1024L*1024L), m_total_memory_used(0) {
  }

  virtual ~TileManager() {
    saveAllTiles();
  }

  void setOptions(int tile_width, int tile_height, int max_memory) {
    // empty anything still stored in cache
    saveAllTiles();
    m_tile_list.clear();
    m_tile_map.clear();

    m_tile_width = tile_width;
    m_tile_height = tile_height;
    m_total_memory_used = 0;
    m_max_memory = max_memory*1024L*1024L;
  }

  template <typename T>
  std::shared_ptr<ImageTile<T>> getTileForPixel(int x, int y, std::shared_ptr<const ImageSource<T>> source) {
    x = x / m_tile_width * m_tile_width;
    y = y / m_tile_height * m_tile_height;

    TileKey key {std::static_pointer_cast<const ImageSourceBase>(source), x, y};
    auto it = m_tile_map.find(key);
    if (it != m_tile_map.end()) {
      //std::cout << "Found in cache!\n";
      return std::dynamic_pointer_cast<ImageTile<T>>(it->second);
    } else {
      auto tile = source->getImageTile(x, y,
          std::min(m_tile_width, source->getWidth()-x), std::min(m_tile_height, source->getHeight()-y));
      addTile(key, std::static_pointer_cast<ImageTileBase>(tile));
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
    auto& tile = m_tile_map.at(tile_key);

    tile->saveIfModified();
    m_total_memory_used -= tile->getTileSize();

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

  void addTile(TileKey key, std::shared_ptr<ImageTileBase> tile) {
    m_tile_map[key] = tile;
    m_tile_list.push_front(key);
    m_total_memory_used += tile->getTileSize();
  }

  int m_tile_width, m_tile_height;
  long m_max_memory;
  long m_total_memory_used;

  std::unordered_map<TileKey, std::shared_ptr<ImageTileBase>> m_tile_map;
  std::list<TileKey> m_tile_list;

  static std::shared_ptr<TileManager> s_instance;
};

}


#endif /* _SEFRAMEWORK_IMAGE_TILEMANAGER_H_ */

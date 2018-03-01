/*
 * TileManager.h
 *
 *  Created on: Feb 23, 2018
 *      Author: mschefer
 */

#ifndef _SEFRAMEWORK_IMAGE_TILEMANAGER_H_
#define _SEFRAMEWORK_IMAGE_TILEMANAGER_H_

#include <list>
#include <map>

#include "SEFramework/Image/ImageTile.h"
#include "SEFramework/Image/ImageSource.h"

namespace SExtractor {

class TileManager {
public:

  struct TileKey {
    unsigned int m_source_id;
    int m_tile_x, m_tile_y;

    bool operator<(const TileKey& other) const {
      return m_source_id < other.m_source_id || (m_source_id == other.m_source_id && m_tile_x < other.m_tile_x)
          || (m_source_id == other.m_source_id && m_tile_x == other.m_tile_x && m_tile_y < other.m_tile_y);
    }
  };

  TileManager() : m_tile_width(64), m_tile_height(64) {
  }

  virtual ~TileManager() = default;

  template <typename T>
  std::shared_ptr<ImageTile<T>> getTileForPixel(int x, int y, const ImageSource<T>& source) {
    TileKey key {source.getId(), x, y};
    auto it = m_tile_map.find(key);
    if (it != m_tile_map.end()) {
      return std::dynamic_pointer_cast<ImageTile<T>>(it->second);
    } else {
      auto tile = source.getImageTile(x, y, m_tile_width, m_tile_height);
      m_tile_map[key] = std::static_pointer_cast<ImageTileBase>(tile);
      return tile;
    }
  }

  static std::shared_ptr<TileManager> getInstance() {
    if (s_instance == nullptr) {
      s_instance = std::make_shared<TileManager>();
    }
    return s_instance;
  }

private:
  int m_tile_width, m_tile_height;
  std::map<TileKey, std::shared_ptr<ImageTileBase>> m_tile_map;
  std::list<std::shared_ptr<ImageTileBase>> m_tile_list;

  static std::shared_ptr<TileManager> s_instance;
};

}

#endif /* _SEFRAMEWORK_IMAGE_TILEMANAGER_H_ */

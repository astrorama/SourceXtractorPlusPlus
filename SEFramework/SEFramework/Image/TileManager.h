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
#include <list>
#include <unordered_map>

#include <boost/thread/shared_mutex.hpp>

#include <ElementsKernel/Logging.h>

#include "SEFramework/Image/ImageTile.h"
#include "SEFramework/Image/ImageSource.h"

namespace SourceXtractor {


struct TileKey {
  std::shared_ptr<const ImageSource> m_source;
  int m_tile_x, m_tile_y;

  bool operator==(const TileKey& other) const;

  std::string getRepr() const;
};

inline std::ostream& operator<<(std::ostream& out, const TileKey& tk) {
  out << tk.getRepr();
  return out;
}

}

namespace std {

template<>
struct hash<SourceXtractor::TileKey> {
  std::size_t operator()(const SourceXtractor::TileKey& key) const {
    std::size_t local_hash = 0;
    boost::hash_combine(local_hash, key.m_source);
    boost::hash_combine(local_hash, key.m_tile_x);
    boost::hash_combine(local_hash, key.m_tile_y);
    return local_hash;
  }
};

}

namespace SourceXtractor {

class TileManager {
public:

  TileManager();

  virtual ~TileManager();

  // Actually not thread safe, call before starting the multi-threading
  void setOptions(int tile_width, int tile_height, int max_memory);

  void flush();

  std::shared_ptr<ImageTile>
  getTileForPixel(int x, int y, std::shared_ptr<const ImageSource> source);

  static std::shared_ptr<TileManager> getInstance();

  void saveAllTiles();

  int getTileWidth() const;

  int getTileHeight() const;

private:

  std::shared_ptr<ImageTile> tryTileFromCache(const TileKey& key);

  std::shared_ptr<boost::mutex>& getMutexForImageSource(const ImageSource*);

  void removeTile(TileKey tile_key);

  void removeExtraTiles();

  void addTile(TileKey key, std::shared_ptr<ImageTile> tile);

  int m_tile_width, m_tile_height;
  long m_max_memory;
  long m_total_memory_used;

  std::unordered_map<TileKey, std::shared_ptr<ImageTile>> m_tile_map;
  std::unordered_map<const ImageSource*, std::shared_ptr<boost::mutex>> m_mutex_map;
  std::list<TileKey> m_tile_list;

  boost::shared_mutex m_mutex;
};

}


#endif /* _SEFRAMEWORK_IMAGE_TILEMANAGER_H_ */

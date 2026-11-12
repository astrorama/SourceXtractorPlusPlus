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

  /// Exclusive write access to a resident tile until the handle is released.
  class WritePin {
  public:
    WritePin() = default;
    WritePin(WritePin&&) noexcept = default;
    WritePin& operator=(WritePin&& other) noexcept;
    ~WritePin();

    /// Return the borrowed tile pointer, or null if this handle has no pin.
    /// The pointer is valid through this handle only while it owns the pin.
    ImageTile* get() const {
      return m_tile.get();
    }

    /// Release the pin without saving the tile.
    void reset() noexcept;

  private:
    friend class TileManager;
    explicit WritePin(std::shared_ptr<ImageTile> tile);

    std::shared_ptr<ImageTile> m_tile;
  };

  TileManager();

  virtual ~TileManager();

  // Actually not thread safe, call before starting the multi-threading
  void setOptions(int tile_width, int tile_height, int max_memory);

  /// Save and clear the cache. Throws if a tile is pinned for writing.
  void flush();

  std::shared_ptr<ImageTile>
  getTileForPixel(int x, int y, std::shared_ptr<const ImageSource> source);

  static std::shared_ptr<TileManager> getInstance();

  /// Save dirty tiles. Throws if a tile is pinned for writing.
  void saveAllTiles();

  int getTileWidth() const;

  int getTileHeight() const;

private:

  template <typename T> friend class WriteableBufferedImage;

  /// Acquire a write pin. Throws if another writer already holds the tile.
  WritePin pinTileForPixel(int x, int y, std::shared_ptr<const ImageSource> source);

  std::shared_ptr<ImageTile>
  getTileForPixel(int x, int y, std::shared_ptr<const ImageSource> source, WritePin* write_pin);
  std::shared_ptr<ImageTile> tryTileFromCache(const TileKey& key, WritePin* write_pin);
  void saveAllTilesLocked();

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

/*
 * BufferedImage.h
 *
 *  Created on: Feb 14, 2018
 *      Author: mschefer
 */

#ifndef _SEFRAMEWORK_IMAGE_BUFFEREDIMAGE_H_
#define _SEFRAMEWORK_IMAGE_BUFFEREDIMAGE_H_

#include <mutex>

#include "SEFramework/Image/ImageBase.h"
#include "SEFramework/Image/ImageSource.h"
#include "SEFramework/Image/ImageTile.h"
#include "SEFramework/Image/TileManager.h"

#include <boost/thread/tss.hpp>

namespace SExtractor {

/**
 *
 */
template <typename T>
class BufferedImage : public ImageBase<T> {
protected:

  BufferedImage(std::shared_ptr<const ImageSource<T>> source, std::shared_ptr<TileManager> tile_manager)
      : m_source(source), m_tile_manager(tile_manager) {}

public:
  virtual ~BufferedImage() = default;

  static std::shared_ptr<BufferedImage<T>> create(std::shared_ptr<const ImageSource<T>> source,
      std::shared_ptr<TileManager> tile_manager = TileManager::getInstance()) {
    return std::shared_ptr<BufferedImage<T>>(new BufferedImage<T>(source, tile_manager));
  }

  virtual std::string getRepr() const override {
    return "BufferedImage(" + m_source->getRepr() + ")";
  }

  /// Returns the value of the pixel with the coordinates (x,y)
  virtual T getValue(int x, int y) const override {
    assert(x >= 0 && y >=0 && x < m_source->getWidth() && y < m_source->getHeight());

    if (m_current_tile == nullptr || !m_current_tile->isPixelInTile(x, y)) {
      m_current_tile = m_tile_manager->getTileForPixel(x, y, m_source);
    }

    return m_current_tile->getValue(x, y);
  }

  /// Returns the width of the image in pixels
  virtual int getWidth() const override {
    return m_source->getWidth();
  }

  /// Returns the height of the image in pixels
  virtual int getHeight() const override {
    return m_source->getHeight();
  }

  virtual std::shared_ptr<ImageChunk<T>> getChunk(int x, int y, int width, int height) const override {
    int tile_width = m_tile_manager->getTileWidth();
    int tile_height = m_tile_manager->getTileHeight();
    int tile_offset_x = x % tile_width;
    int tile_offset_y = y % tile_height;

    // When the chunk does *not* cross boundaries, we can just use the memory hold by the single tile
    if (tile_offset_x + width <= tile_width && tile_offset_y + height <= tile_height) {
      // the tile image is going to be kept in memory as long as the chunk exists, but it could be unloaded
      // from TileManager and even reloaded again, wasting memory,
      // however image chunks are normally short lived so it's probably OK
      auto tile = m_tile_manager->getTileForPixel(x, y, m_source);
      // The tile may be smaller than tile_width x tile_height if the image is smaller, or does not divide neatly!
      const T *data_start = &(tile->getImage()->getData()[tile_offset_x +
                                                          tile_offset_y * tile->getImage()->getWidth()]);
      return ImageChunk<T>::create(data_start, width, height, tile->getImage()->getWidth(), tile->getImage());
    } else {
      // If the chunk cross boundaries, we can't just use the memory from within a tile, so we need to copy
      // To avoid the overhead of calling getValue() - which uses a thread local - we do the full thing here
      // Also, instead of iterating on the pixel coordinates, to avoid asking several times for the same tile,
      // iterate over the tiles
      std::vector<T> data(width * height);
      int tile_w = m_tile_manager->getTileWidth();
      int tile_h = m_tile_manager->getTileHeight();

      int tile_start_x = x / tile_w * tile_w;
      int tile_start_y = y / tile_h * tile_h;
      int tile_end_x = (x + width - 1) / tile_w * tile_w;
      int tile_end_y = (y + height - 1) / tile_h * tile_h;

      for (int iy = tile_start_y; iy <= tile_end_y; iy += tile_h) {
        for (int ix = tile_start_x; ix <= tile_end_x; ix += tile_w) {
          auto tile = m_tile_manager->getTileForPixel(ix, iy, m_source);
          copyOverlappingPixels(*tile, data, x, y, width, height, tile_w, tile_h);
        }
      }

      return UniversalImageChunk<T>::create(std::move(data), width, height);
    }
  }

protected:
  std::shared_ptr<const ImageSource<T>> m_source;
  std::shared_ptr<TileManager> m_tile_manager;
  mutable std::shared_ptr<ImageTile<T>> m_current_tile;

  void copyOverlappingPixels(const ImageTile<T> &tile, std::vector<T> &output,
                             int x, int y, int w, int h,
                             int tile_w, int tile_h) const {
    int start_x = std::max(tile.getPosX(), x);
    int start_y = std::max(tile.getPosY(), y);
    int end_x = std::min(tile.getPosX() + tile_w, x + w);
    int end_y = std::min(tile.getPosY() + tile_h, y + h);
    int off_x = start_x - x;
    int off_y = start_y - y;

    for (int data_y = off_y, img_y = start_y; img_y < end_y; ++data_y, ++img_y) {
      for (int data_x = off_x, img_x = start_x; img_x < end_x; ++data_x, ++img_x) {
        output[data_x + data_y * w] = tile.getValue(img_x, img_y);
      }
    }
  }
};

}


#endif /* _SEFRAMEWORK_IMAGE_BUFFEREDIMAGE_H_ */

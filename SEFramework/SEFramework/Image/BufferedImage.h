/*
 * BufferedImage.h
 *
 *  Created on: Feb 14, 2018
 *      Author: mschefer
 */

#ifndef _SEFRAMEWORK_IMAGE_BUFFEREDIMAGE_H_
#define _SEFRAMEWORK_IMAGE_BUFFEREDIMAGE_H_

#include "SEFramework/Image/ImageBase.h"
#include "SEFramework/Image/ImageSource.h"
#include "SEFramework/Image/ImageTile.h"
#include "SEFramework/Image/TileManager.h"

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

  /// Returns the value of the pixel with the coordinates (x,y)
  virtual T getValue(int x, int y) const override {
    //std::cout << "BufferedImage::getValue() " << x << " " << y << "\n";
    assert(x >= 0 && y >=0 && x < m_source->getWidth() && y < m_source->getHeight());

    if (m_current_tile == nullptr || !m_current_tile->isPixelInTile(x, y)) {
      const_cast<BufferedImage<T>*>(this)->m_current_tile = m_tile_manager->getTileForPixel(x, y, m_source);
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

    if (width == tile_width && height == tile_height && (x % tile_width) == 0 && (y % tile_height) == 0) {
      // the tile image is going to be kept in memory as long as the chunk exists, but it could be unloaded
      // from TileManager and even reloaded again, wasting memory,
      // however image chunks are normally short lived so it's probably OK
      auto tile = m_tile_manager->getTileForPixel(x, y, m_source);
      return ImageChunk<T>::create(&tile->getImage()->getData()[0], width, height, width, tile->getImage());
    } else {
      // TODO implement optimized version of getting chunks that are not aligned with tiles
      return UniversalImageChunk<T>::create(this->shared_from_this(), x, y, width, height);
    }
  }

protected:
  std::shared_ptr<const ImageSource<T>> m_source;
  std::shared_ptr<TileManager> m_tile_manager;
  std::shared_ptr<ImageTile<T>> m_current_tile;
};

}


#endif /* _SEFRAMEWORK_IMAGE_BUFFEREDIMAGE_H_ */

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

protected:
  std::shared_ptr<const ImageSource<T>> m_source;
  std::shared_ptr<TileManager> m_tile_manager;
  std::shared_ptr<ImageTile<T>> m_current_tile;
};

}


#endif /* _SEFRAMEWORK_IMAGE_BUFFEREDIMAGE_H_ */

/*
 * WriteableBufferedImage.h
 *
 *  Created on: Mar 8, 2018
 *      Author: mschefer
 */

#ifndef _SEFRAMEWORK_IMAGE_WRITEABLEBUFFEREDIMAGE_H_
#define _SEFRAMEWORK_IMAGE_WRITEABLEBUFFEREDIMAGE_H_

#include "SEFramework/Image/WriteableImage.h"
#include "SEFramework/Image/BufferedImage.h"

namespace SExtractor {

template <typename T>
class WriteableBufferedImage : public BufferedImage<T>, public WriteableImage<T> {

protected:

  WriteableBufferedImage(std::shared_ptr<const ImageSource<T>> source, std::shared_ptr<TileManager> tile_manager)
      : BufferedImage<T>(source, tile_manager) {}

public:

  virtual ~WriteableBufferedImage() = default;

  static std::shared_ptr<WriteableBufferedImage<T>> create(std::shared_ptr<const ImageSource<T>> source,
      std::shared_ptr<TileManager> tile_manager = TileManager::getInstance()) {
    return std::shared_ptr<WriteableBufferedImage<T>>(new WriteableBufferedImage<T>(source, tile_manager));
  }

  virtual void setValue(int x, int y, T value) override {
    assert(x >= 0 && y >=0 && x < BufferedImage<T>::m_source->getWidth() && y < BufferedImage<T>::m_source->getHeight());

    if (BufferedImage<T>::m_current_tile == nullptr || !BufferedImage<T>::m_current_tile->isPixelInTile(x, y)) {
      BufferedImage<T>::m_current_tile =
          BufferedImage<T>::m_tile_manager->getTileForPixel(x, y, BufferedImage<T>::m_source);
    }

    BufferedImage<T>::m_current_tile->setModified(true);
    BufferedImage<T>::m_current_tile->setValue(x, y, value);
  }

};

}

#endif /* _SEFRAMEWORK_IMAGE_WRITEABLEBUFFEREDIMAGE_H_ */

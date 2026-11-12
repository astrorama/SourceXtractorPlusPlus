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
 * WriteableBufferedImage.h
 *
 *  Created on: Mar 8, 2018
 *      Author: mschefer
 */

#ifndef _SEFRAMEWORK_IMAGE_WRITEABLEBUFFEREDIMAGE_H_
#define _SEFRAMEWORK_IMAGE_WRITEABLEBUFFEREDIMAGE_H_

#include "SEFramework/Image/WriteableImage.h"
#include "SEFramework/Image/BufferedImage.h"

namespace SourceXtractor {

template <typename T>
class WriteableBufferedImage : public BufferedImage<T>, public virtual WriteableImage<T> {

protected:

  WriteableBufferedImage(std::shared_ptr<const ImageSource> source, std::shared_ptr<TileManager> tile_manager)
      : BufferedImage<T>(source, tile_manager) {}

public:

  virtual ~WriteableBufferedImage() = default;

  static std::shared_ptr<WriteableBufferedImage<T>> create(std::shared_ptr<const ImageSource> source,
      std::shared_ptr<TileManager> tile_manager = TileManager::getInstance()) {
    return std::shared_ptr<WriteableBufferedImage<T>>(new WriteableBufferedImage<T>(source, tile_manager));
  }

  void setValue(int x, int y, T value) override {
    assert(x >= 0 && y >=0 && x < BufferedImage<T>::m_source->getWidth() && y < BufferedImage<T>::m_source->getHeight());

    if (m_write_batch_depth == 0) {
      auto pin = pinTile(x, y);
      writePixel(*pin.get(), x, y, value);
      return;
    }

    if (!m_write_pin.get() || !m_write_pin.get()->isPixelInTile(x, y)) {
      m_write_pin.reset();
      m_write_pin = pinTile(x, y);
    }

    writePixel(*m_write_pin.get(), x, y, value);
  }

  using BufferedImage<T>::getRepr;

private:
  void beginWriteBatch() noexcept override {
    ++m_write_batch_depth;
  }

  void endWriteBatch() noexcept override {
    if (--m_write_batch_depth == 0) {
      m_write_pin.reset();
    }
  }

  TileManager::WritePin pinTile(int x, int y) {
    return this->m_tile_manager->pinTileForPixel(x, y, this->m_source);
  }

  static void writePixel(ImageTile& tile, int x, int y, T value) {
    tile.setModified(true);
    tile.setValue(x, y, value);
  }

  TileManager::WritePin m_write_pin;
  unsigned int m_write_batch_depth = 0;
};

}

#endif /* _SEFRAMEWORK_IMAGE_WRITEABLEBUFFEREDIMAGE_H_ */

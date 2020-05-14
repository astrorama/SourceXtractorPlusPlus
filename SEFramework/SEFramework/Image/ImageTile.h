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
 * ImageTile.hm
 *
 *  Created on: Feb 20, 2018
 *      Author: mschefer
 */

#ifndef _SEFRAMEWORK_IMAGE_IMAGETILE_H_
#define _SEFRAMEWORK_IMAGE_IMAGETILE_H_

#include "SEFramework/Image/Image.h"
#include "SEFramework/Image/VectorImage.h"

namespace SourceXtractor {

template<typename T> class ImageSource;

class ImageTileBase {
public:
  ImageTileBase() : m_modified(false) {}

  virtual ~ImageTileBase() = default;

  virtual int getTileSize() const = 0;

  void setModified(bool modified) {
    m_modified = modified;
  }

  bool isModified() const {
    return m_modified;
  }

  virtual void saveIfModified() = 0;

private:
  bool m_modified;
};

template <typename T>
class ImageTile : public ImageTileBase {
public:
  ImageTile(std::shared_ptr<ImageSource<T>> source, int x, int y, int width, int height) : m_source(source), m_x(x), m_y(y), m_max_x(x+width), m_max_y(y+height) {
    m_tile_image = VectorImage<T>::create(width, height);
  }

  ImageTile(int x, int y, int width, int height) : m_source(nullptr), m_x(x), m_y(y), m_max_x(x+width), m_max_y(y+height) {
    m_tile_image = VectorImage<T>::create(width, height);
  }

  virtual ~ImageTile() override {
    saveIfModified();
  }

  bool isPixelInTile(int x, int y) const {
    return x >= m_x && y >= m_y && x < m_max_x && y < m_max_y;
  }

  T getValue(int x, int y) const {
    assert(isPixelInTile(x,y));

    return m_tile_image->getValue(x-m_x, y-m_y);
  }

  void setValue(int x, int y, T value) {
    assert(isPixelInTile(x,y));

    return m_tile_image->setValue(x-m_x, y-m_y, value);
  }

  std::shared_ptr<VectorImage<T>>& getImage() {
    return m_tile_image;
  }

  int getTileSize() const override {
    return (m_max_x-m_x) * (m_max_y-m_y) * sizeof(T);
  }

  virtual void saveIfModified() override {
    if (isModified()) {
      m_source->saveTile(*this);
      setModified(false);
    }
  }

  int getPosX() const {
    return m_x;
  }

  int getPosY() const {
    return m_y;
  }

private:
  std::shared_ptr<ImageSource<T>> m_source;
  int m_x, m_y;
  int m_max_x, m_max_y;
  std::shared_ptr<VectorImage<T>> m_tile_image;
};


}


#endif /* _SEFRAMEWORK_IMAGE_IMAGETILE_H_ */

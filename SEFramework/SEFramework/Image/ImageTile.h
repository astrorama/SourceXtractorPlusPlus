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
#include <iostream>
#include "SEFramework/Image/Image.h"
#include "SEFramework/Image/VectorImage.h"

namespace SourceXtractor {

class ImageSource;

class ImageTile {
public:

  enum ImageType {
    AutoType=-1,
    FloatImage=0,
    DoubleImage,
    IntImage,
    UIntImage,
    LongLongImage,
  };

  static std::shared_ptr<ImageTile> create(ImageType image_type, int x, int y, int width, int height, std::shared_ptr<ImageSource> source=nullptr);

  virtual ~ImageTile() {
    saveIfModified();
  }

  bool isPixelInTile(int x, int y) const {
    return x >= m_x && y >= m_y && x < m_max_x && y < m_max_y;
  }

  int getPosX() const {
    return m_x;
  }

  int getPosY() const {
    return m_y;
  }

  virtual int getTileMemorySize() const = 0;

  int getWidth() const {
    return m_max_x - m_x;
  }

  int getHeight() const {
    return m_max_y - m_y;
  }

  template<typename T>
  T getValue(int x, int y) const {
    T value;
    getValue(x, y, value);
    return value;
  }

  virtual void setValue(int x, int y, float value) = 0;
  virtual void setValue(int x, int y, double value) = 0;
  virtual void setValue(int x, int y, int value) = 0;
  virtual void setValue(int x, int y, unsigned int value) = 0;
  virtual void setValue(int x, int y, std::int64_t value) = 0;

  template<typename T>
  std::shared_ptr<VectorImage<T>> getImage() const {
    if (m_image_type == getTypeValue(T())) {
      return std::static_pointer_cast<VectorImage<T>>(m_tile_image);
    } else {
      //FIXME implement type conversion !!!!!!!!!!!!!!!!
      return nullptr;
    }
  }

  virtual void* getDataPtr()=0;

  void setModified(bool modified) {
    m_modified = modified;
  }

  bool isModified() const {
    return m_modified;
  }

  virtual void saveIfModified();

  static ImageType getTypeValue(float) {
    return FloatImage;
  }

  static ImageType getTypeValue(double) {
    return DoubleImage;
  }

  static ImageType getTypeValue(int) {
    return IntImage;
  }

  static ImageType getTypeValue(unsigned int) {
    return UIntImage;
  }

  static ImageType getTypeValue(std::int64_t) {
    return LongLongImage;
  }

  static size_t getTypeSize(ImageType image_type) {
    switch (image_type) {
    default:
    case ImageTile::FloatImage:
    case ImageTile::IntImage:
    case ImageTile::UIntImage:
      return 4;
    case ImageTile::LongLongImage:
    case ImageTile::DoubleImage:
      return 8;
    }
  }

  ImageType getType() const {
    return m_image_type;
  }

protected:
  virtual void getValue(int x, int y, float& value) const = 0;
  virtual void getValue(int x, int y, double& value) const = 0;
  virtual void getValue(int x, int y, int& value) const = 0;
  virtual void getValue(int x, int y, unsigned int& value) const = 0;
  virtual void getValue(int x, int y, std::int64_t& value) const = 0;


  ImageTile(ImageType image_type, int x, int y, int width, int height, std::shared_ptr<ImageSource> source=nullptr)
      : m_modified(false), m_image_type(image_type), m_source(source), m_x(x), m_y(y), m_max_x(x+width), m_max_y(y+height) {
    createImage(image_type, width, height);
  }

  void createImage(ImageType image_type, int width, int height) {
    //std::cout << "create tile type " << image_type << "\n";
    switch (image_type) {
    default:
    case FloatImage:
      m_tile_image = VectorImage<float>::create(width, height);
      break;
    case DoubleImage:
      m_tile_image = VectorImage<double>::create(width, height);
      break;
    case IntImage:
      m_tile_image = VectorImage<int>::create(width, height);
      break;
    case UIntImage:
      m_tile_image = VectorImage<unsigned int>::create(width, height);
      break;
    case LongLongImage:
      m_tile_image = VectorImage<std::int64_t>::create(width, height);
      break;
    }
  }

  bool m_modified;

  ImageType m_image_type;

  std::shared_ptr<ImageSource> m_source;
  int m_x, m_y;
  int m_max_x, m_max_y;

  std::shared_ptr<void> m_tile_image;
};


}


#endif /* _SEFRAMEWORK_IMAGE_IMAGETILE_H_ */

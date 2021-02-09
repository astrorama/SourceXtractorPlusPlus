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

#include "SEFramework/Image/ImageBase.h"
#include "SEFramework/Image/ImageSource.h"
#include "SEFramework/Image/ImageTile.h"

namespace SourceXtractor {

template<typename T>
class ImageTileImpl : public ImageTile {
public:

  ImageTileImpl(ImageType image_type, int x, int y, int width, int height, std::shared_ptr<ImageSource> source)
      : ImageTile(image_type, x, y, width, height, source) {
  }


  virtual int getTileMemorySize() const  {
    return getWidth() * getHeight() * sizeof(T);
  }

  virtual void* getDataPtr() {
    return &std::static_pointer_cast<VectorImage<T>>(m_tile_image)->getData()[0];
  }

  template<typename U>
  void getValueImpl(int x, int y, U& value) const {
    assert(isPixelInTile(x,y));
    auto image = std::static_pointer_cast<VectorImage<T>>(m_tile_image);
    value = image->getValue(x-m_x, y-m_y);
  }

  template<typename U>
  void setValueImpl(int x, int y, U value) {
    assert(isPixelInTile(x,y));
    auto image = std::static_pointer_cast<VectorImage<T>>(m_tile_image);
    image->setValue(x-m_x, y-m_y, value);
  }

  virtual void getValue(int x, int y, float& value) const {
    getValueImpl(x, y, value);
  }

  virtual void getValue(int x, int y, double& value) const {
    getValueImpl(x, y, value);
  }

  virtual void getValue(int x, int y, int& value) const {
    getValueImpl(x, y, value);
  }

  virtual void getValue(int x, int y, unsigned int& value) const {
    getValueImpl(x, y, value);
  }

  virtual void getValue(int x, int y, std::int64_t& value) const {
    getValueImpl(x, y, value);
  }

  virtual void setValue(int x, int y, float value) {
    setValueImpl(x, y, value);
  }

  virtual void setValue(int x, int y, double value)  {
    setValueImpl(x, y, value);
  }

  virtual void setValue(int x, int y, int value)  {
    setValueImpl(x, y, value);
  }

  virtual void setValue(int x, int y, unsigned int value)  {
    setValueImpl(x, y, value);
  }

  virtual void setValue(int x, int y, std::int64_t value) {
    setValueImpl(x, y, value);
  }
};

std::shared_ptr<ImageTile> ImageTile::create(ImageType image_type, int x, int y, int width, int height, std::shared_ptr<ImageSource> source) {
  switch (image_type) {
  default:
  case FloatImage:
    return std::make_shared<ImageTileImpl<float>>(image_type, x, y, width, height, source);
  case DoubleImage:
    return std::make_shared<ImageTileImpl<double>>(image_type, x, y, width, height, source);
  case IntImage:
    return std::make_shared<ImageTileImpl<int>>(image_type, x, y, width, height, source);
  case UIntImage:
    return std::make_shared<ImageTileImpl<unsigned int>>(image_type, x, y, width, height, source);
  case LongLongImage:
    return std::make_shared<ImageTileImpl<std::int64_t>>(image_type, x, y, width, height, source);
  }
}

void ImageTile::saveIfModified() {
  if (isModified()) {
    m_source->saveTile(*this);
    setModified(false);
  }
}

}



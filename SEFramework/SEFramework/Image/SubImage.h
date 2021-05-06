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

#ifndef _SEFRAMEWORK_IMAGE_SUBIMAGE_H_
#define _SEFRAMEWORK_IMAGE_SUBIMAGE_H_

#include <memory>

#include "SEFramework/Image/Image.h"
#include "SEFramework/Image/ImageAccessor.h"

namespace SourceXtractor {

/**
 * @class SubImage
 * @brief Part of another image
 */

template<typename T>
class SubImage : public Image<T> {
protected:
  SubImage(std::shared_ptr<const Image<T>> image, const PixelCoordinate &offset, int width, int height)
    : m_image(image), m_offset(offset), m_width(width), m_height(height) {
    assert(offset.m_x >= 0 && offset.m_y >= 0 && width > 0 && height > 0 &&
        offset.m_x + width <= image->getWidth() && offset.m_y + height <= image->getHeight());
  }

  SubImage(std::shared_ptr<const Image<T>> image, int x, int y, int width, int height)
    : m_image(image), m_offset(x, y), m_width(width), m_height(height) {
    assert(x >= 0 && y >= 0 && width > 0 && height > 0 &&
        x + width <= image->getWidth() && y + height <= image->getHeight());
  }

public:
  /**
   * @brief Destructor
   */
  virtual ~SubImage() = default;

  template<typename... Args>
  static std::shared_ptr<SubImage<T>> create(Args &&... args) {
    return std::shared_ptr<SubImage<T>>(new SubImage{std::forward<Args>(args)...});
  }

  std::string getRepr() const override {
    return "SubImage(" + m_image->getRepr() + ", " + std::to_string(m_offset.m_x) + ", " + std::to_string(m_offset.m_y) + ", " + std::to_string(m_width) + ", " + std::to_string(m_height) + ")";
  }

  int getWidth() const override {
    return m_width;
  }

  int getHeight() const override {
    return m_height;
  }

  std::shared_ptr<ImageChunk<T>> getChunk(int x, int y, int width, int height) const override {
    ImageAccessor<T> accessor(m_image);
    auto chunk = UniversalImageChunk<T>::create(width, height);
    for (int iy = 0; iy < height; ++iy) {
      for (int ix = 0; ix < width; ++ix) {
        chunk->at(ix, iy) = accessor.getValue(ix + x + m_offset.m_x, iy + y + m_offset.m_y);
      }
    }
    return chunk;
  }

private:
  std::shared_ptr<const Image<T>> m_image;
  PixelCoordinate m_offset;
  int m_width, m_height;
};

} /* namespace SourceXtractor */

#endif /* _SEFRAMEWORK_IMAGE_SUBIMAGE_H_ */

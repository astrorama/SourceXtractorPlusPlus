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
#include "SEFramework/Image/ImageBase.h"

namespace SourceXtractor {

/**
 * @class SubImage
 * @brief Extracts part of an image
 *
 */

template <typename T>
class SubImage : public ImageBase<T> {

protected:
  SubImage(std::shared_ptr<const Image<T>> image, int x, int y, int width, int height)
      : m_image(image), m_x(x), m_y(y), m_width(width), m_height(height) {
    //TODO add asserts
  };

public:

  /**
   * @brief Destructor
   */
  virtual ~SubImage() = default;

  static std::shared_ptr<SubImage<T>> create(
      std::shared_ptr<const Image<T>> image, int x, int y, int width, int height) {
    return std::shared_ptr<SubImage<T>>(new SubImage<T>(image, x, y, width, height));
  }

  std::string getRepr() const override {
    return "SubImage(" + m_image->getRepr() + ", " + std::to_string(m_x) + ", " + std::to_string(m_y) + ", " + std::to_string(m_width) + ", " + std::to_string(m_height) + ")";
  }

  using Image<T>::getValue;
  T getValue(int x, int y) const override {
    return m_image->getValue(m_x + x, m_y + y);
  }

  int getWidth() const override {
    return m_width;
  }

  int getHeight() const override {
    return m_height;
  }

private:
  std::shared_ptr<const Image<T>> m_image;
  int m_x, m_y;
  int m_width, m_height;
};

} /* namespace SourceXtractor */


#endif /* _SEFRAMEWORK_IMAGE_SUBIMAGE_H_ */

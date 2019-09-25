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
 * MultiplyImage.h
 *
 *  Created on: Apr 25, 2017
 *      Author: mschefer
 */

#ifndef _SEFRAMEWORK_IMAGE_MULTIPLYIMAGE_H_
#define _SEFRAMEWORK_IMAGE_MULTIPLYIMAGE_H_

#include <memory>

#include "SEFramework/Image/Image.h"
#include "SEFramework/Image/ImageBase.h"
#include "SEFramework/Image/ConstantImage.h"

namespace SExtractor {

template <typename T>
class MultiplyImage : public ImageBase<T> {

protected:

  MultiplyImage(std::shared_ptr<const Image<T>> image, std::shared_ptr<const Image<T>> image_to_multiply)
      : m_image(image), m_image_to_multiply(image_to_multiply) {
    assert(m_image->getWidth() == m_image_to_multiply->getWidth());
    assert(m_image->getHeight() == m_image_to_multiply->getHeight());
  };

public:

  static std::shared_ptr<MultiplyImage<T>> create(
      std::shared_ptr<const Image<T>> image, std::shared_ptr<const Image<T>> image_to_multiply) {
    return std::shared_ptr<MultiplyImage<T>>(new MultiplyImage<T>(image, image_to_multiply));
  }

  static std::shared_ptr<MultiplyImage<T>> create(std::shared_ptr<const Image<T>> image, T value_to_multiply) {
    return std::shared_ptr<MultiplyImage<T>>(new MultiplyImage<T>(
        image, ConstantImage<T>::create(image->getWidth(), image->getHeight(), value_to_multiply)));
  }

  /**
   * @brief Destructor
   */
  virtual ~MultiplyImage() = default;

  std::string getRepr() const override {
    return "MultiplyImage(" + m_image->getRepr() + "," + m_image_to_multiply->getRepr() + ")";
  }

  using Image<T>::getValue;
  T getValue(int x, int y) const override {
    return m_image->getValue(x, y) * m_image_to_multiply->getValue(x, y);
  }

  int getWidth() const override {
    return m_image->getWidth();
  }

  int getHeight() const override {
    return m_image->getHeight();
  }

private:
  std::shared_ptr<const Image<T>> m_image, m_image_to_multiply;

};

} /* namespace SExtractor */



#endif /* _SEFRAMEWORK_IMAGE_MULTIPLYIMAGE_H_ */

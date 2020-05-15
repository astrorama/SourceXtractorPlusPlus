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
 * ConstantImage.h
 *
 *  Created on: Jan 12, 2017
 *      Author: mschefer
 */

#ifndef _SEFRAMEWORK_IMAGE_CONSTANTIMAGE_H_
#define _SEFRAMEWORK_IMAGE_CONSTANTIMAGE_H_

#include "SEFramework/Image/Image.h"
#include "SEFramework/Image/ImageBase.h"

namespace SourceXtractor {

template <typename T>
class ConstantImage : public ImageBase<T> {
protected:

  ConstantImage(int width, int height, T constant_value)
      : m_width(width), m_height(height), m_constant_value(constant_value) {}


public:

  static std::shared_ptr<ConstantImage<T>> create(int width, int height, T constant_value) {
    return std::shared_ptr<ConstantImage<T>>(new ConstantImage(width, height, constant_value));
  }

  virtual ~ConstantImage() = default;

  std::string getRepr() const final {
    return "ConstantImage<" + std::to_string(m_width) + "," + std::to_string(m_height) + ">(" +
           std::to_string(m_constant_value) + ")";
  }

  int getHeight() const final {
    return m_height;
  }

  int getWidth() const final {
    return m_width;
  }

  using Image<T>::getValue;
  T getValue(int, int) const final {
    return m_constant_value;
  }

  std::shared_ptr<ImageChunk<T>> getChunk(int /*x*/, int /*y*/, int width, int height) const final {
    return UniversalImageChunk<T>::create(std::vector<T>(width * height, m_constant_value), width, height);
  }

private:

  int m_width;
  int m_height;
  T m_constant_value;

};

}


#endif /* _SEFRAMEWORK_IMAGE_CONSTANTIMAGE_H_ */

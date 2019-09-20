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
 * @file SEFramework/Image/FunctionalImage.h
 * @date 27/03/19
 * @author Alejandro Alvarez Ayllon
 */

#ifndef _SEFRAMEWORK_IMAGE_FUNCTIONALIMAGE_H
#define _SEFRAMEWORK_IMAGE_FUNCTIONALIMAGE_H

#include "SEFramework/Image/ImageBase.h"

namespace SExtractor {

template<typename T>
class FunctionalImage : public ImageBase<T> {
public:
  using FunctorType = std::function<T(int x, int y)>;

protected:
  FunctionalImage(int width, int height, FunctorType functor)
    : m_width{width}, m_height{height}, m_functor{functor} {
  }

public:
  virtual ~FunctionalImage() = default;

  template<typename ...Args>
  static std::shared_ptr<ImageBase<T>> create(Args&& ... args) {
    return std::shared_ptr<FunctionalImage<T>>(new FunctionalImage{std::forward<Args>(args)...});
  }

  std::string getRepr() const override {
    return "FunctionalImage<" + std::string(m_functor.target_type().name()) + ">";
  }

  T getValue(int x, int y) const override {
    return m_functor(x, y);
  }

  int getWidth() const override {
    return m_width;
  }

  int getHeight() const override {
    return m_height;
  }

private:
  int m_width, m_height;
  FunctorType m_functor;
};

} // end SExtractor

#endif // _SEFRAMEWORK_IMAGE_FUNCTIONALIMAGE_H

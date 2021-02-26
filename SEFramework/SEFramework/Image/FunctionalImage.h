/** Copyright © 2019-2021 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
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

#include "SEFramework/Image/Image.h"
#include "SEFramework/Image/ImageChunk.h"

namespace SourceXtractor {

/**
 * Apply a transformation to each individual pixel
 * @tparam T
 *  Pixel type
 * @tparam
 *  Pixel type of the input image
 * @warning
 *  Make sure the functor does not capture anything that may be outlived
 *  by the functional image. This is hardly surprising.
 *  Also, make sure that whatever is captured is thread safe!
 */
template<typename T, typename I = T>
class FunctionalImage : public Image<T> {
public:
  using FunctorType = std::function<T(int x, int y, I v)>;

protected:
  FunctionalImage(std::shared_ptr<const Image<I>> img, FunctorType functor)
    : m_img{std::move(img)}, m_functor{functor} {
  }

public:
  virtual ~FunctionalImage() = default;

  template<typename ...Args>
  static std::shared_ptr<FunctionalImage<T, I>> create(Args&& ... args) {
    return std::shared_ptr<FunctionalImage<T, I>>(new FunctionalImage{std::forward<Args>(args)...});
  }

  std::string getRepr() const final {
    return "FunctionalImage<" + std::string(m_functor.target_type().name()) + ">";
  }

  int getWidth() const final {
    return m_img->getWidth();
  }

  int getHeight() const final {
    return m_img->getHeight();
  }

  std::shared_ptr<ImageChunk<T>> getChunk(int x, int y, int width, int height) const final {
    auto in_chunk = m_img->getChunk(x, y, width, height);
    auto chunk = UniversalImageChunk<T>::create(width, height);
    for (int iy = 0; iy < height; ++iy) {
      for (int ix = 0; ix < width; ++ix) {
        chunk->at(ix, iy) = m_functor(ix + x, iy + y, in_chunk->getValue(ix, iy));
      }
    }
    return chunk;
  }

private:
  std::shared_ptr<const Image<I>> m_img;
  FunctorType m_functor;
};

} // end SourceXtractor

#endif // _SEFRAMEWORK_IMAGE_FUNCTIONALIMAGE_H

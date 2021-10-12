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
 * @file SEFramework/Image/ReflectImage.h
 * @date 11/09/18
 * @author Alejandro Alvarez Ayllon
 */

#ifndef _SEFRAMEWORK_IMAGE_MIRRORIMAGE_H
#define _SEFRAMEWORK_IMAGE_MIRRORIMAGE_H

#include "SEFramework/Image/Image.h"
#include "SEFramework/Image/ImageAccessor.h"
#include "SEFramework/Image/ImageChunk.h"

namespace SourceXtractor {

/**
 * @class MirrorImage
 * @brief Mirrors an image in both X and Y axes
 */
template <typename T>
class MirrorImage: public Image<T> {
protected:
  explicit MirrorImage(std::shared_ptr<const Image<T>> img) : m_img{img} {
  }

public:
  template<typename... Args>
  static std::shared_ptr<MirrorImage<T>> create(Args &&... args) {
    return std::shared_ptr<MirrorImage<T>>(new MirrorImage{std::forward<Args>(args)...});
  }

  std::string getRepr() const override {
    return "MirrorImage(" + m_img->getRepr() + ")";
  }

  int getWidth() const override {
    return m_img->getWidth();
  }

  int getHeight() const override {
    return m_img->getHeight();
  }

  std::shared_ptr<ImageChunk<T>> getChunk(int x, int y, int width, int height) const override {
    ImageAccessor<T> accessor(m_img, ImageAccessor<T>::BOTTOM_RIGHT);
    auto chunk = UniversalImageChunk<T>::create(width, height);
    auto img_w = accessor.getWidth();
    auto img_h = accessor.getHeight();
    for (int iy = 0; iy < height; ++iy) {
      for (int ix = 0; ix < width; ++ix) {
        chunk->at(ix, iy) = accessor.getValue(img_w - (x + ix) - 1, img_h - (y + iy) - 1);
      }
    }
    return chunk;
  }

private:
  std::shared_ptr<const Image<T>> m_img;
};

} // end SourceXtractor

#endif // _SEFRAMEWORK_IMAGE_MIRRORIMAGE_H

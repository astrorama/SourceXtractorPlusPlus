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
 * @file SEFramework/Image/WarpImage.h
 * @date 11/09/18
 * @author Alejandro Alvarez Ayllon
 */

#ifndef _SEFRAMEWORK_IMAGE_WARPIMAGE_H
#define _SEFRAMEWORK_IMAGE_WARPIMAGE_H

#include "SEFramework/Image/Image.h"
#include "SEFramework/Image/ImageAccessor.h"

namespace SourceXtractor {

/**
 * @class RecenterImage
 * @brief Changes the center of an image, wrapping it around the edges
 */
template<typename T>
class RecenterImage : public Image<T> {
protected:
  RecenterImage(std::shared_ptr<const Image<T>> img, const PixelCoordinate &new_center) : m_img{img},
                                                                                          m_center{new_center} {
  }

public:
  template<typename... Args>
  static std::shared_ptr<RecenterImage<T>> create(Args &&... args) {
    return std::shared_ptr<RecenterImage<T>>(new RecenterImage{std::forward<Args>(args)...});
  }

  std::string getRepr() const override {
    return "RecenterImage(" + m_img->getRepr() + ")";
  }

  int getWidth() const override {
    return m_img->getWidth();
  }

  int getHeight() const override {
    return m_img->getHeight();
  }

  std::shared_ptr<const VectorImage<T>> getChunk(int x, int y, int width, int height) const override {
    auto output = VectorImage<T>::create(width, height);
    getChunk(x, y, *output);
    return output;
  }

  void getChunk(int x, int y, VectorImage<T>& output) const override {
    int              width  = output.getWidth();
    int              height = output.getHeight();
    ImageAccessor<T> accessor(m_img);
    auto             img_w = accessor.getWidth();
    auto             img_h = accessor.getHeight();
    for (int iy = 0; iy < height; ++iy) {
      for (int ix = 0; ix < width; ++ix) {
        output.at(ix, iy) = accessor.getValue((ix + x + m_center.m_x) % img_w, (iy + y + m_center.m_y) % img_h);
      }
    }
  }

private:
  std::shared_ptr<const Image<T>> m_img;
  PixelCoordinate m_center;
};

} // end SourceXtractor

#endif // _SEFRAMEWORK_IMAGE_WARPIMAGE_H

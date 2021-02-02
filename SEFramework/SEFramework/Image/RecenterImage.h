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

  T getValue(int x, int y) const override {
    x = (x + m_center.m_x) % m_img->getWidth();
    y = (y + m_center.m_y) % m_img->getHeight();
    return m_img->getValue(x, y);
  }

  std::shared_ptr<ImageChunk<T>> getChunk(int x, int y, int width, int height) const override{
    abort();
  }

private:
  std::shared_ptr<const Image<T>> m_img;
  PixelCoordinate m_center;
};

} // end SourceXtractor

#endif // _SEFRAMEWORK_IMAGE_WARPIMAGE_H

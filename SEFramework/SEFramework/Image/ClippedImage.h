/** Copyright © 2021 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
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

#ifndef _SEFRAMEWORK_IMAGE_CLIPPEDIMAGE_H_
#define _SEFRAMEWORK_IMAGE_CLIPPEDIMAGE_H_

#include "SEFramework/Image/Image.h"

namespace SourceXtractor {

/**
 * Clip an image
 * @tparam T
 *  Pixel type
 */
template <typename T>
class ClippedImage: public Image<T> {
private:

  /**
   * Constructor
   * @param img
   *    Image to decorate
   * @param x
   *    New x start coordinate
   * @param y
   *    New y start coordinate
   * @param w
   *    New width
   * @param h
   *    New height
   */
  ClippedImage(std::shared_ptr<const Image<T>> img, int x, int y, int w, int h)
  : m_img{img}, m_clip_x{x}, m_clip_y{y}, m_clip_w{w}, m_clip_h{h} {
    assert(img->getWidth() - m_clip_x >= m_clip_w);
    assert(img->getHeight() - m_clip_y >= m_clip_h);
  }

public:

  template <typename ...Args>
  static std::shared_ptr<ClippedImage<T>> create(Args&& ...args) {
    return std::shared_ptr<ClippedImage<T>>(new ClippedImage<T>(std::forward<Args>(args)...));
  }

  std::string getRepr() const override {
    return "Clipped(" + m_img->getRepr() + ")";
  }

  int getWidth() const override {
    return m_clip_w;
  }

  int getHeight() const override {
    return m_clip_h;
  }

  std::shared_ptr<ImageChunk<T>> getChunk(int x, int y, int width, int height) const override {
    return m_img->getChunk(x + m_clip_x, y + m_clip_y, width, height);
  }

private:
  std::shared_ptr<const Image<T>> m_img;
  int m_clip_x, m_clip_y;
  int m_clip_w, m_clip_h;
};

} // end of namespace SourceXtractor

#endif // _SEFRAMEWORK_IMAGE_CLIPPEDIMAGE_H_

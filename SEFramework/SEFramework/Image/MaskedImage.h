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
* MaskedImage.h
*
*  Created on: Feb 13, 2020
*      Author: Alejandro Alvarez Ayllon
*/

#ifndef SEFRAMEWORK_SEFRAMEWORK_IMAGE_MASKEDIMAGE_H_
#define SEFRAMEWORK_SEFRAMEWORK_IMAGE_MASKEDIMAGE_H_

#include "SEFramework/Image/ImageBase.h"

namespace SourceXtractor {

/**
 * Mask an image, replacing masked values
 * @tparam T
 *  Pixel type of the masked image
 * @tparam M
 *  Pixel type of the mask
 * @tparam Operator
 *  Operator to apply on the mask pixels. Defaults to bitwise and (&)
 *
 * @note
 *  The image is masked where the operator evaluates to true on the mask image
 */
template<typename T, typename M, typename Operator = std::bit_and<M>>
class MaskedImage : public ImageBase<T> {
private:
  MaskedImage(const std::shared_ptr<Image<T>>& image, const std::shared_ptr<Image<M>>& mask,
              T replacement, M mask_flag) : m_image{image}, m_mask{mask}, m_replacement{replacement},
                                            m_mask_flag{mask_flag} {
  }

  std::shared_ptr<Image<T>> m_image;
  std::shared_ptr<Image<M>> m_mask;
  T m_replacement;
  M m_mask_flag;
  Operator m_operator;

public:
  virtual ~MaskedImage() = default;

  /**
   * Create a masked image
   * @param image
   *    Image to be masked
   * @param mask
   *    Mask
   * @param replacement
   *    Replace masked pixels with this value
   * @param mask_flag
   *    If Operator(mask pixel, mask_flag) is true, this given pixel is replaces
   * @return
   *    A new masked image
   */
  static std::shared_ptr<MaskedImage<T, M, Operator>>
  create(const std::shared_ptr<Image<T>>& image, const std::shared_ptr<Image<M>>& mask,
         T replacement, M mask_flag = 0x01) {
    assert(image->getWidth() == mask->getWidth() && image->getHeight() == mask->getHeight());
    return std::shared_ptr<MaskedImage<T, M, Operator>>(new MaskedImage<T, M, Operator>(image, mask, replacement, mask_flag));
  }

  std::string getRepr() const override {
    return std::string("Masked(" + m_image->getRepr() + ")");
  }

  T getValue(int x, int y) const override {
    if (m_operator(m_mask->getValue(x, y), m_mask_flag))
      return m_replacement;
    return m_image->getValue(x, y);
  }

  int getWidth() const override {
    return m_image->getWidth();
  }

  int getHeight() const override {
    return m_image->getHeight();
  }
};

} // end of namespace SourceXtractor

#endif // SEFRAMEWORK_SEFRAMEWORK_IMAGE_MASKEDIMAGE_H_

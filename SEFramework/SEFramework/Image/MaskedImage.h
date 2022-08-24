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

#include "SEFramework/Image/Image.h"
#include "SEFramework/Image/ImageChunk.h"

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
template<typename T, typename M, template <typename> class Operator = std::bit_and>
class MaskedImage : public Image<T> {
private:
  MaskedImage(const std::shared_ptr<Image<T>>& image, const std::shared_ptr<Image<M>>& mask,
              T replacement, M mask_flag) : m_image{image}, m_mask{mask}, m_replacement{replacement},
                                            m_mask_flag{mask_flag}, m_n_masked{0}, m_n_unmasked{0} {
  }

  std::shared_ptr<Image<T>> m_image;
  std::shared_ptr<Image<M>> m_mask;
  T m_replacement;
  M m_mask_flag;
  Operator<M> m_operator;

  mutable std::size_t m_n_masked;
  mutable std::size_t m_n_unmasked;

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
   *    If Operator(mask pixel, mask_flag) is true, this given pixel is replaced
   * @return
   *    A new masked image
   */
  static std::shared_ptr<MaskedImage<T, M, Operator>>
  create(const std::shared_ptr<Image<T>>& image, const std::shared_ptr<Image<M>>& mask,
         T replacement, M mask_flag = 0x01) {
    assert(image->getWidth() == mask->getWidth() && image->getHeight() == mask->getHeight());
    return std::shared_ptr<MaskedImage<T, M, Operator>>(new MaskedImage<T, M, Operator>(image, mask, replacement, mask_flag));
  }

  std::string getRepr() const final {
    char char_fract[8];
    // determine the percentage of masked pixels
    std::snprintf(char_fract, 7, "%.1f%", 100.0*this->getNMasked()/(m_image->getWidth()*m_image->getHeight()));
    return std::string("Masked(" + m_image->getRepr() + ") with "+std::string(char_fract)+" masked pixels");
  }

  int getWidth() const final {
    return m_image->getWidth();
  }

  int getHeight() const final {
    return m_image->getHeight();
  }

  std::size_t getNMasked() const {
    return m_n_masked;
  }

  std::size_t getNUnMasked() const {
    return m_n_unmasked;
  }

  std::shared_ptr<ImageChunk<T>> getChunk(int x, int y, int width, int height) const final {
    auto chunk = UniversalImageChunk<T>::create(std::move(*m_image->getChunk(x, y, width, height)));
    auto mask_chunk = m_mask->getChunk(x, y, width, height);
    for (int iy = 0; iy < height; ++iy) {
      for (int ix = 0; ix < width; ++ix) {
        if (m_operator(mask_chunk->getValue(ix, iy), m_mask_flag)){
          chunk->setValue(ix, iy, m_replacement);
          m_n_masked+=1;
        }
        else {
          m_n_unmasked+=1;
        }
      }
    }
    return chunk;
  }

};

} // end of namespace SourceXtractor

#endif // SEFRAMEWORK_SEFRAMEWORK_IMAGE_MASKEDIMAGE_H_

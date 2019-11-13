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
 * @file SEFramework/Convolution/DirectConvolution.h
 * @date 17/09/18
 * @author aalvarez
 */

#ifndef _SEFRAMEWORK_CONVOLUTION_DIRECTCONVOLUTION_H
#define _SEFRAMEWORK_CONVOLUTION_DIRECTCONVOLUTION_H

#include "SEFramework/Image/MirrorImage.h"
#include "SEFramework/Image/PaddedImage.h"
#include "SEFramework/Image/VectorImage.h"

namespace SExtractor {

template <typename T = SeFloat, class TPadding = PaddedImage<T, Reflect101Coordinates>>
class DirectConvolution {
public:
  DirectConvolution(std::shared_ptr<const Image<T>> img)
    : m_kernel{MirrorImage<T>::create(img)} {
  }

  virtual ~DirectConvolution() = default;

  template <typename ...Args>
  void convolve(std::shared_ptr<WriteableImage<T>> image, Args... padding_args) const {
    auto padded_width = image->getWidth() + m_kernel->getWidth() - 1;
    auto padded_height = image->getHeight() + m_kernel->getHeight() - 1;
    auto lpad = m_kernel->getWidth() / 2;
    auto tpad = m_kernel->getHeight() / 2;

    auto padded = VectorImage<T>::create(
      TPadding::create(image, padded_width, padded_height, std::forward<Args>(padding_args)...)
    );

    for (int iy = tpad; iy < padded->getHeight() - tpad; ++iy) {
      for (int ix = lpad; ix < padded->getWidth() - lpad; ++ix) {
        T acc = 0;
        for (int ky = 0; ky < m_kernel->getHeight(); ++ky) {
          for (int kx = 0; kx < m_kernel->getWidth(); ++kx) {
            acc += m_kernel->getValue(kx, ky) * padded->getValue(ix - lpad + kx, iy - tpad + ky);
          }
        }
        image->setValue(ix - lpad, iy - tpad, acc);
      }
    }
  }

  std::size_t getWidth() const {
    return m_kernel->getWidth();
  }

  std::size_t getHeight() const {
    return m_kernel->getHeight();
  }

  std::shared_ptr<const Image<T>> getKernel() const {
    return m_kernel;
  }

private:
  std::shared_ptr<const Image<T>> m_kernel;
};

} // end SExtractor

#endif // _SEFRAMEWORK_CONVOLUTION_DIRECTCONVOLUTION_H

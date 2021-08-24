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

#ifndef _SEFRAMEWORK_FFT_FFTHELPER_H
#define _SEFRAMEWORK_FFT_FFTHELPER_H

#include "SEFramework/Image/Image.h"
#include <cassert>
#include <vector>

namespace SourceXtractor {

/**
 * Initialize the buffer with the pixel values in origin taking into
 * account the memory layout expected by an in-place FFT transform.
 * @see
 *  http://www.fftw.org/fftw3_doc/Multi_002dDimensional-DFTs-of-Real-Data.html
 * @tparam T
 *  Pixel type
 * @tparam Img
 *  An image type
 * @param origin
 *  Copy pixels from this image
 * @param buffer
 *  Into this buffer
 * @warning
 *  The size of the buffer *must* fit the padded data used by FFTW3: height * (width / 2 + 1) * 2
 */
template <typename T, template <typename...> class Img>
static void copyImageToFFTWorkArea(Img<T>& origin, std::vector<T>& buffer) {
  int width  = origin.getWidth();
  int height = origin.getHeight();
  int pad    = 2 * (width / 2 + 1) - width;
  int stride = width + pad;
  assert(buffer.size() == static_cast<size_t>(height * (width + pad)));
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      buffer[x + y * stride] = origin.getValue(x, y);
    }
  }
}

/**
 * Copy the data from a buffer following FFTW in-place memory layout (padded)
 * into an image
 * @see
 *  copyImageToFFTWorkArea
 * @tparam T
 *  Pixel type
 * @tparam Img
 *  An image type
 * @param buffer
 *  FFTW output data
 * @param dest
 *  Image to initialize with the buffer data
 * @param lpad
 *  (Optional, defaults to 0) For convolving, images are usually padded. When extracting the data,
 *  it is considered to be horizontally padded with this many pixels *left*
 * @param rpad
 *  (Optional, defaults to 0) For convolving, images are usually padded. When extracting the data,
 *  it is considered to be horizontally padded with this many pixels *right*
 * @param tpad
 *  (Optional, defaults to 0) For convolving, images are usually padded. When extracting the data,
 *  it is considered to be vertically padded with this many pixels *top*
 * @param bpad
 *  (Optional, defaults to 0) For convolving, images are usually padded. When extracting the data,
 *  it is considered to be vertically padded with this many pixels *bottom*
 * @param normalize
 *  (Optional, defaults to true) After a convolution, the output values are scaled by N (number of *logical* pixels on the input)
 *  If true, scale-back dividing by N
 */
template <typename T, template <typename...> class Img>
static void copyFFTWorkAreaToImage(std::vector<T>& buffer, Img<T>& dest, int rpad = 0, int lpad = 0, int tpad = 0, int bpad = 0,
                                   bool normalize = true) {
  const int width         = dest.getWidth();
  const int height        = dest.getHeight();
  const int padded_width  = width + rpad + lpad;
  const int padded_height = height + tpad + bpad;
  const int fftw_pad      = 2 * (padded_width / 2 + 1) - padded_width;
  const int stride        = padded_width + fftw_pad;
  const int total_size    = normalize ? padded_width * padded_height : 1;
  assert(buffer.size() == static_cast<size_t>(padded_height * (padded_width + fftw_pad)));
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      dest.setValue(x, y, buffer[x + lpad + (y + tpad) * stride] / total_size);
    }
  }
}

}  // namespace SourceXtractor

#endif  // _SEFRAMEWORK_FFT_FFTHELPER_H

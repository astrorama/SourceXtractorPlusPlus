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

#ifndef SOURCEXTRACTORPLUSPLUS_MEDIANFILTER_H
#define SOURCEXTRACTORPLUSPLUS_MEDIANFILTER_H

#include "SEFramework/Image/VectorImage.h"
#include "SEFramework/Image/WriteableImage.h"
#include <array>

namespace SourceXtractor {

/**
 * Functor that applies a median filter over a pair of image/variance image
 * @tparam T Pixel type of the filtered image
 * @details
 *  A median filter takes as parameters the width and height of the smoothing box. Each pixel
 *  is computed as the median of the values of the pixels contained within a box centered at
 *  its coordinates *iff* the median value differs from the actual value by more than a given threshold.
 *
 *  If the value of the pixel is replaced, the value of the variance at the same coordinates is
 *  replaced also by the median of the variances contained within the box.
 *
 *  There is no padding. At the edges whe box size is *symmetrically* clipped to the number of available pixels.
 */
template<typename T>
class MedianFilter {
public:
  /**
   * Constructor
   * @param box_width
   * @param box_height
   */
  MedianFilter(int box_width, int box_height) : m_box_width(box_width), m_box_height(box_height) {
  }

  /**
   * Constructor
   * @param box
   *    A two dimensional array where the first value corresponds to the width, and the second to the height.
   */
  explicit MedianFilter(const std::array<int, 2>& box) : MedianFilter(box[0], box[1]) {
  }

  /**
   * Call operator
   * @param image
   *    Image to filter
   * @param variance
   *    Associated variance map
   * @param threshold
   *    The pixel is replaced only if the median and the actual value differ by at least this value.
   *    Defaults to 0 (always replaced).
   * @return
   *    A pair of new images containing the filtered image and the filtered variance map
   */
  auto operator()(const VectorImage<T>& image, const VectorImage<T>& variance,
                  T threshold = 0) const -> std::pair<std::shared_ptr<VectorImage<T>>, std::shared_ptr<VectorImage<T>>> {
    assert(image.getWidth() == variance.getWidth());
    assert(image.getHeight() == variance.getHeight());

    auto out_img = VectorImage<T>::create(image.getWidth(), image.getHeight());
    auto out_var = VectorImage<T>::create(image.getWidth(), image.getHeight());

    for (int y = 0; y < image.getHeight(); ++y) {
      for (int x = 0; x < image.getWidth(); ++x) {
        auto box = getBox(image, x, y);
        auto median = getMedian(box);
        auto value = image.getValue(x, y);
        if (std::abs(median - value) >= threshold) {
          out_img->setValue(x, y, median);
          auto var_box = getBox(variance, x, y);
          out_var->setValue(x, y, getMedian(var_box));
        }
        else {
          out_img->setValue(x, y, value);
          out_var->setValue(x, y, variance.getValue(x, y));
        }
      }
    }

    return std::make_pair(out_img, out_var);
  }

private:
  int m_box_width, m_box_height;

  /**
   * Convenience method to compute the median of a vector
   * @param data
   *    The vector on which to compute the median. *It will be modified*.
   */
  static T getMedian(std::vector<T>& data) {
    std::sort(data.begin(), data.end());
    auto nitems = data.size();
    if (nitems % 2 == 1)
      return data[nitems / 2];
    return (data[nitems / 2] + data[nitems / 2 - 1]) / 2;
  }

  /**
   * Symmetrically clip the size of the smoothing box depending on the number of available pixels.
   * This is a 1-D version that is applied to X and Y separately.
   * @param position
   *    Coordinate where the box will be centered
   * @param box_size
   *    Size of the smoothing box
   * @param image_size
   *    Image size
   * @return
   *    As many pixels as could be read safely from the image, up to box_size
   */
  static int clip(int position, int box_size, int image_size) {
    box_size /= 2;
    if (box_size > position)
      return position;
    if (box_size > image_size - position - 1)
      return image_size - position - 1;
    return box_size;
  }

  /**
   * Get the pixel values contained within a box centered at x,y
   */
  std::vector<T> getBox(const VectorImage<T>& img, int x, int y) const {
    int hw = clip(x, m_box_width, img.getWidth());
    int hh = clip(y, m_box_height, img.getHeight());
    std::vector<T> data;
    auto inserter = std::back_inserter(data);
    for (int iy = -hh; iy < hh + 1; ++iy) {
      for (int ix = -hw; ix < hw + 1; ++ix) {
        *inserter = img.getValue(x + ix, y + iy);
        ++inserter;
      }
    }
    return data;
  }
};

} // end of namespace SourceXtractor

#endif //SOURCEXTRACTORPLUSPLUS_MEDIANFILTER_H

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

#include "SEImplementation/Background/SE/ReplaceUndefImage.h"

namespace SourceXtractor {

template<typename T>
ReplaceUndefImage<T>::ReplaceUndefImage(const std::shared_ptr<VectorImage<T>>& image, T invalid)
  : m_image{image}, m_invalid{invalid} {
}

template<typename T>
std::string ReplaceUndefImage<T>::getRepr() const {
  return std::string("ReplaceUndef(" + m_image->getRepr() + ")");
}

template<typename T>
int ReplaceUndefImage<T>::getWidth() const {
  return m_image->getWidth();
}

template<typename T>
int ReplaceUndefImage<T>::getHeight() const {
  return m_image->getHeight();
}


template<typename T>
static T getMaskedValue(int x, int y, const Image<T>& img, T invalid) {
  auto v = img.getValue(x, y);
  if (v != invalid)
    return v;

  auto min_distance = std::numeric_limits<T>::max();

  T acc = 0;
  size_t count = 0;

  for (int iy = 0; iy < img.getHeight(); ++iy) {
    for (int ix = 0; ix < img.getWidth(); ++ix) {
      v = img.getValue(ix, iy);
      if (v != invalid) {
        auto dx = x - ix;
        auto dy = y - iy;
        auto distance = dx * dx + dy * dy;
        // This pixel is closer than the last close one, so restart
        if (distance < min_distance) {
          acc = v;
          count = 1;
          min_distance = distance;
        }
          // This pixel is as close as the closest one, so take it into account
        else if (distance - min_distance <= std::numeric_limits<T>::epsilon()) {
          acc += v;
          ++count;
        }
      }
    }
  }

  // Take the average
  if (count > 0)
    acc /= count;

  return acc;
}

template<typename T>
T ReplaceUndefImage<T>::getValue(int x, int y) const {
  return getMaskedValue<T>(x, y, *m_image, m_invalid);
}

template<typename T>
std::shared_ptr<ImageChunk<T>> ReplaceUndefImage<T>::getChunk(
  int x, int y, int width, int height) const {
  auto chunk = UniversalImageChunk<T>::create(std::move(*m_image->getChunk(x, y, width, height)));
  for (int iy = 0; iy < height; ++iy) {
    for (int ix = 0; ix < width; ++ix) {
      chunk->at(ix, iy) = getMaskedValue(ix, iy, *chunk, m_invalid);
    }
  }
  return chunk;
}

template
class ReplaceUndefImage<SeFloat>;

} // end of namespace SourceXtractor

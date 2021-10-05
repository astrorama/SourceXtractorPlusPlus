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
static T getMaskedValue(int x, int y, const VectorImage<T>& img, T invalid) {
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
std::shared_ptr<VectorImage<T>> ReplaceUndef(const VectorImage<T>& original, T mask) {
  auto output = VectorImage<T>::create(original.getWidth(), original.getHeight());
  for (int y = 0; y < original.getHeight(); ++y) {
    for (int x = 0; x < original.getWidth(); ++x) {
      output->at(x, y) = getMaskedValue(x, y, original, mask);
    }
  }
  return output;
}

// Instantiation
template std::shared_ptr<VectorImage<SeFloat>> ReplaceUndef(const VectorImage<SeFloat>&, SeFloat);

} // end of namespace SourceXtractor

#include "SEFramework/Image/ImageAccessor.h"

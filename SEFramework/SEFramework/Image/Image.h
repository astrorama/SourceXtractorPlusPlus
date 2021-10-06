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
/**
 * @file SEFramework/Image/Image.h
 * @date 05/30/16
 * @author mschefer
 */

#ifndef _SEFRAMEWORK_IMAGE_IMAGE_H
#define _SEFRAMEWORK_IMAGE_IMAGE_H

#include <memory>

#include "SEUtils/Types.h"
#include "SEUtils/PixelCoordinate.h"

namespace SourceXtractor {

template <typename T>
class VectorImage;


/**
 * @class Image
 * @brief Interface representing an image
 * @tparam T the type of the pixel values
 */
template <typename T>
class Image {

public:

  using PixelType = T;

  /**
   * @brief Destructor
   */
  virtual ~Image() = default;

  /// Get a string identifying this image in a human readable manner
  virtual std::string getRepr() const = 0;

  /// Returns the width of the image in pixels
  virtual int getWidth() const = 0;

  /// Returns the height of the image in pixels
  virtual int getHeight() const = 0;

  virtual std::shared_ptr<VectorImage<T>> getChunk(int x, int y, int width, int height) const = 0;

  std::shared_ptr<VectorImage<T>> getChunk(const PixelCoordinate& start,
                                          const PixelCoordinate& end) const {
    assert(isInside(start.m_x, start.m_y) && isInside(end.m_x, end.m_y));
    return getChunk(start.m_x, start.m_y, end.m_x - start.m_x + 1, end.m_y - start.m_y + 1);
  }

  /// Returns true if the given coordinates are inside the image bounds
  bool isInside(int x, int y) const {
    return x >= 0 && y >= 0 && x < getWidth() && y < getHeight();
  }

}; /* End of Image class */

/// Alias for the detection image, to make easier its type modification
using DetectionImage = Image<SeFloat>;

/// Alias for the measurement image, to make easier its type modification
using MeasurementImage = Image<SeFloat>;

/// Alias for the weight image, to make easier its type modification
using WeightImage = Image<SeFloat>;

/// Alias for the flag image, to make easier its type modification
using FlagImage = Image<std::int64_t>;

} /* namespace SourceXtractor */

#endif

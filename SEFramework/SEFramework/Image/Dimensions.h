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
 * @file Dimensions.h
 * @date Nov 18, 2025
 * @author embray
 *
 * The extents of an image, and the order in which they are indexed.
 */

#ifndef _SEFRAMEWORK_IMAGE_DIMENSIONS_H
#define _SEFRAMEWORK_IMAGE_DIMENSIONS_H

#include <vector>

namespace SourceXtractor {

/// Order in which the axes of a Dimensions are indexed
enum class AxisOrder {
  XY,  ///< fastest axis first, as FITS reports NAXISn
  YX,  ///< slowest axis first, as a C/Python/NumPy shape
};

/**
 * The extents of an image
 *
 * Indexing through toVector() follows the axis order, so the named accessors
 * are the unambiguous way to ask for one extent. Only ever two or three axes:
 * nothing in SourceXtractor++ reads an image of higher rank.
 */
class Dimensions {

public:

  Dimensions(int width, int height, int depth = 0, AxisOrder axis_order = AxisOrder::YX)
    : m_width(width), m_height(height), m_depth(depth), m_axis_order(axis_order) {}

  int getWidth() const {
    return m_width;
  }

  int getHeight() const {
    return m_height;
  }

  /// Number of layers, or zero if this is not a data cube
  int getDepth() const {
    return m_depth;
  }

  AxisOrder getAxisOrder() const {
    return m_axis_order;
  }

  bool isCube() const {
    return m_depth > 0;
  }

  /// The extents in axis order, three elements for a cube and two otherwise
  std::vector<int> toVector() const {
    if (m_axis_order == AxisOrder::XY) {
      if (isCube()) {
        return {m_width, m_height, m_depth};
      }
      return {m_width, m_height};
    }

    if (isCube()) {
      return {m_depth, m_height, m_width};
    }
    return {m_height, m_width};
  }

  bool operator==(const Dimensions& other) const {
    return m_width == other.m_width && m_height == other.m_height &&
           m_depth == other.m_depth && m_axis_order == other.m_axis_order;
  }

  bool operator!=(const Dimensions& other) const {
    return !(*this == other);
  }

private:
  int m_width;
  int m_height;
  int m_depth;
  AxisOrder m_axis_order;
};

}  // namespace SourceXtractor

#endif /* _SEFRAMEWORK_IMAGE_DIMENSIONS_H */

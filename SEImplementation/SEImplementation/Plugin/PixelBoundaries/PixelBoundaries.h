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
 * @file SEImplementation/PixelBoundaries.h
 * @date 05/30/16
 * @author mschefer
 */

#ifndef _SEIMPLEMENTATION_PROPERTY_PIXELBOUNDARIES_H
#define _SEIMPLEMENTATION_PROPERTY_PIXELBOUNDARIES_H

#include "SEUtils/PixelCoordinate.h"

#include "SEFramework/Property/Property.h"

namespace SExtractor {

/**
 * @class PixelBoundaries
 * @brief The bounding box of all the pixels in the source. Both min and max coordinate are inclusive.
 *
 */
class PixelBoundaries : public Property {

public:

  /**
   * @brief Destructor
   */
  virtual ~PixelBoundaries() = default;

  PixelBoundaries(int min_x, int min_y, int max_x, int max_y) : m_min(min_x, min_y), m_max(max_x, max_y) {
  }

  PixelCoordinate getMin() const {
    return m_min;
  }

  PixelCoordinate getMax() const {
    return m_max;
  }

  int getWidth() const {
    return m_max.m_x - m_min.m_x + 1;
  }

  int getHeight() const {
    return m_max.m_y - m_min.m_y + 1;
  }

private:
  PixelCoordinate m_min, m_max;

}; /* End of PixelBoundaries class */


class PixelBoundariesHalfMaximum : public PixelBoundaries {

public:

  /**
   * @brief Destructor
   */
  virtual ~PixelBoundariesHalfMaximum() = default;

  PixelBoundariesHalfMaximum(int min_x, int min_y, int max_x, int max_y)
      : PixelBoundaries(min_x, min_y, max_x, max_y) {
  }
};


} /* namespace SExtractor */


#endif

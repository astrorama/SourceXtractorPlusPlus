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

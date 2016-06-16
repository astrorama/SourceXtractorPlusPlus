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
 * @brief
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

private:
  PixelCoordinate m_min, m_max;

}; /* End of PixelBoundaries class */



} /* namespace SEImplementation */


#endif

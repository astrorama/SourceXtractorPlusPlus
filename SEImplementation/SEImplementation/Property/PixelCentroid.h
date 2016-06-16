/**
 * @file SEImplementation/PixelCentroid.h
 * @date 05/27/16
 * @author mschefer
 */

#ifndef _SEIMPLEMENTATION_PROPERTY_PIXELCENTROID_H
#define _SEIMPLEMENTATION_PROPERTY_PIXELCENTROID_H

#include "SEFramework/Property/Property.h"

namespace SExtractor {

/**
 * @class PixelCentroid
 * @brief
 *
 */
class PixelCentroid : public Property {
public:

  /**
   * @brief Destructor
   */
  virtual ~PixelCentroid() = default;

  PixelCentroid(double centroid_x, double centroid_y) : m_centroid_x(centroid_x), m_centroid_y(centroid_y) {}

  double getCentroidX() const {
    return m_centroid_x;
  }

  double getCentroidY() const {
    return m_centroid_y;
  }

private:
  double m_centroid_x, m_centroid_y;

}; /* End of PixelCentroid class */

} /* namespace SEImplementation */


#endif

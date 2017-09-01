/**
 * @file SEImplementation/MeasurementFramePixelCentroid.h
 * @date 10/28/16
 * @author mschefer
 */

#ifndef _SEIMPLEMENTATION_PROPERTY_MEASUREMENTFRAMEPIXELCENTROID_H
#define _SEIMPLEMENTATION_PROPERTY_MEASUREMENTFRAMEPIXELCENTROID_H

#include "SEUtils/Types.h"
#include "SEFramework/Property/Property.h"

namespace SExtractor {

class MeasurementFramePixelCentroid : public Property {
public:

  /**
   * @brief Destructor
   */
  virtual ~MeasurementFramePixelCentroid() = default;

  MeasurementFramePixelCentroid(SeFloat centroid_x, SeFloat centroid_y) : m_centroid_x(centroid_x), m_centroid_y(centroid_y) {}

  /// X coordinate of centroid
  SeFloat getCentroidX() const {
    return m_centroid_x;
  }

  /// Y coordinate of centroid
  SeFloat getCentroidY() const {
    return m_centroid_y;
  }

private:
  SeFloat m_centroid_x, m_centroid_y;

};

} /* namespace SExtractor */


#endif

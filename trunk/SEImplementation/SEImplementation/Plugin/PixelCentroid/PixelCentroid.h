/**
 * @file SEImplementation/PixelCentroid.h
 * @date 05/27/16
 * @author mschefer
 */

#ifndef _SEIMPLEMENTATION_PROPERTY_PIXELCENTROID_H
#define _SEIMPLEMENTATION_PROPERTY_PIXELCENTROID_H

#include "SEUtils/Types.h"
#include "SEFramework/Property/Property.h"

namespace SExtractor {

/**
 * @class PixelCentroid
 * @brief The centroid of all the pixels in the source, weighted by their DetectionImage pixel values.
 */
class PixelCentroid : public Property {
public:

  /**
   * @brief Destructor
   */
  virtual ~PixelCentroid() = default;

  PixelCentroid(SeFloat centroid_x, SeFloat centroid_y) : m_centroid_x(centroid_x), m_centroid_y(centroid_y) {}

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

}; /* End of PixelCentroid class */

} /* namespace SExtractor */


#endif

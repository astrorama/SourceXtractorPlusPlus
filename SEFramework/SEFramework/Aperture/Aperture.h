/*
 * Aperture.h
 *
 *  Created on: Oct 08, 2018
 *      Author: Alejandro Alvarez
 */

#ifndef _SEFRAMEWORK_SEFRAMEWORK_APERTURE_APERTURE_H
#define _SEFRAMEWORK_SEFRAMEWORK_APERTURE_APERTURE_H

#include "SEUtils/PixelCoordinate.h"
#include "SEUtils/Types.h"

namespace SExtractor {

class Aperture {
public:
  virtual ~Aperture() = default;

  virtual SeFloat getArea(SeFloat center_x, SeFloat center_y, SeFloat pixel_x, SeFloat pixel_y) const = 0;

  virtual PixelCoordinate getMinPixel(SeFloat centroid_x, SeFloat centroid_y) const = 0;

  virtual PixelCoordinate getMaxPixel(SeFloat centroid_x, SeFloat centroid_y) const = 0;

  virtual SeFloat getRadiusSquared(SeFloat center_x, SeFloat center_y, SeFloat pixel_x, SeFloat pixel_y) const = 0;
};

} // end SExtractor

#endif // _SEFRAMEWORK_SEFRAMEWORK_APERTURE_APERTURE_H

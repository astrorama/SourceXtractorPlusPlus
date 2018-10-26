/*
 * CircularAperture.h
 *
 *  Created on: Oct 08, 2018
 *      Author: Alejandro Alvarez
 */

#ifndef _SEFRAMEWORK_SEFRAMEWORK_APERTURE_CIRCULARAPERTURE_H
#define _SEFRAMEWORK_SEFRAMEWORK_APERTURE_CIRCULARAPERTURE_H

#include "Aperture.h"

namespace SExtractor {

class CircularAperture : public Aperture {
public:
  virtual ~CircularAperture() = default;

  CircularAperture(SeFloat radius) : m_radius(radius) {}

  SeFloat getArea(SeFloat center_x, SeFloat center_y, SeFloat pixel_x, SeFloat pixel_y) const override;

  PixelCoordinate getMinPixel(SeFloat centroid_x, SeFloat centroid_y) const override;

  PixelCoordinate getMaxPixel(SeFloat centroid_x, SeFloat centroid_y) const override;

  SeFloat getRadiusSquared(SeFloat center_x, SeFloat center_y, SeFloat pixel_x, SeFloat pixel_y) const override;

private:
  SeFloat m_radius;
};

} // end SExtractor

#endif // _SEFRAMEWORK_SEFRAMEWORK_APERTURE_CIRCULARAPERTURE_H

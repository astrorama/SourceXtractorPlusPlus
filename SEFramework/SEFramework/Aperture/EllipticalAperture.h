/*
 * EllipticalAperture.h
 *
 *  Created on: Oct 08, 2018
 *      Author: Alejandro Alvarez
 */

#ifndef _SEFRAMEWORK_SEFRAMEWORK_APERTURE_ELLIPTICALAPERTURE_H
#define _SEFRAMEWORK_SEFRAMEWORK_APERTURE_ELLIPTICALAPERTURE_H

#include "Aperture.h"

namespace SExtractor {

class EllipticalAperture : public Aperture {
public:
  virtual ~EllipticalAperture() = default;

  EllipticalAperture(SeFloat cxx, SeFloat cyy, SeFloat cxy, SeFloat rad_max);

  SeFloat getArea(SeFloat center_x, SeFloat center_y, int pixel_x, int pixel_y) const override;

  PixelCoordinate getMinPixel(SeFloat centroid_x, SeFloat centroid_y) const override;

  PixelCoordinate getMaxPixel(SeFloat centroid_x, SeFloat centroid_y) const override;

  SeFloat getRadiusSquared(SeFloat center_x, SeFloat center_y, int pixel_x, int pixel_y) const override;

private:
  SeFloat m_cxx;
  SeFloat m_cyy;
  SeFloat m_cxy;
  SeFloat m_rad_max;
};

} // end SExtractor

#endif // _SEFRAMEWORK_SEFRAMEWORK_APERTURE_ELLIPTICALAPERTURE_H

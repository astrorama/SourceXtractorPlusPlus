/*
 * TransformedAperture.h
 *
 *  Created on: Oct 08, 2018
 *      Author: Alejandro Alvarez
 */

#ifndef _SEFRAMEWORK_SEFRAMEWORK_APERTURE_TRANSFORMEDAPERTURE_H
#define _SEFRAMEWORK_SEFRAMEWORK_APERTURE_TRANSFORMEDAPERTURE_H

#include "Aperture.h"

namespace SExtractor {

class TransformedAperture: public Aperture {
public:
  virtual ~TransformedAperture() = default;

  TransformedAperture(std::shared_ptr<Aperture> decorated, const std::tuple<double, double, double, double> &jacobian);

  SeFloat getArea(SeFloat center_x, SeFloat center_y, SeFloat pixel_x, SeFloat pixel_y) const override;

  PixelCoordinate getMinPixel(SeFloat centroid_x, SeFloat centroid_y) const override;

  PixelCoordinate getMaxPixel(SeFloat centroid_x, SeFloat centroid_y) const override;

  SeFloat getRadiusSquared(SeFloat center_x, SeFloat center_y, SeFloat pixel_x, SeFloat pixel_y) const override;

private:
  std::shared_ptr<Aperture> m_decorated;
  std::array<double, 4> m_transform, m_inv_transform;
};

} // end SExtractor

#endif // _SEFRAMEWORK_SEFRAMEWORK_APERTURE_TRANSFORMEDAPERTURE_H

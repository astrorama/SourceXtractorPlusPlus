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
/*
 * EllipticalAperture.h
 *
 *  Created on: Oct 08, 2018
 *      Author: Alejandro Alvarez
 */

#ifndef _SEFRAMEWORK_SEFRAMEWORK_APERTURE_ELLIPTICALAPERTURE_H
#define _SEFRAMEWORK_SEFRAMEWORK_APERTURE_ELLIPTICALAPERTURE_H

#include "Aperture.h"

namespace SourceXtractor {

class EllipticalAperture : public Aperture {
public:
  virtual ~EllipticalAperture() = default;

  EllipticalAperture(SeFloat cxx, SeFloat cyy, SeFloat cxy, SeFloat rad_max);

  SeFloat getArea(SeFloat center_x, SeFloat center_y, SeFloat pixel_x, SeFloat pixel_y) const override;

  PixelCoordinate getMinPixel(SeFloat centroid_x, SeFloat centroid_y) const override;

  PixelCoordinate getMaxPixel(SeFloat centroid_x, SeFloat centroid_y) const override;

  SeFloat getRadiusSquared(SeFloat center_x, SeFloat center_y, SeFloat pixel_x, SeFloat pixel_y) const override;

private:
  SeFloat m_cxx;
  SeFloat m_cyy;
  SeFloat m_cxy;
  SeFloat m_rad_max;
};

} // end SourceXtractor

#endif // _SEFRAMEWORK_SEFRAMEWORK_APERTURE_ELLIPTICALAPERTURE_H

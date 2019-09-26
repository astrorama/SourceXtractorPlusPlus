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

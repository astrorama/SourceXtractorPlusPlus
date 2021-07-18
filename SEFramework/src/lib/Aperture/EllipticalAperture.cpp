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
 * EllipticalAperture.cpp
 *
 *  Created on: Oct 08, 2018
 *      Author: Alejandro Alvarez
 */

#include "SEFramework/Aperture/EllipticalAperture.h"
#include <iostream>
namespace SourceXtractor {


EllipticalAperture::EllipticalAperture(SeFloat cxx, SeFloat cyy, SeFloat cxy,
                                       SeFloat rad_max)
  : m_cxx{cxx}, m_cyy{cyy}, m_cxy{cxy}, m_rad_max{rad_max} {
}

  SeFloat EllipticalAperture::getArea(SeFloat center_x, SeFloat center_y, SeFloat pixel_x, SeFloat pixel_y) const {
  	if (getRadiusSquared(center_x, center_y, pixel_x, pixel_y) < m_rad_max * m_rad_max) {

  		return 1.0;
  	}
  	return 0.;
  }

  SeFloat EllipticalAperture::drawArea(SeFloat center_x, SeFloat center_y, SeFloat pixel_x, SeFloat pixel_y) const {
	  SeFloat thickness = 1.0;
	  SeFloat rad_min_squared = m_rad_max > thickness ? (m_rad_max - thickness) * (m_rad_max - thickness) : 0;

	  auto distance_squared = getRadiusSquared(center_x, center_y, pixel_x, pixel_y);

	  //if (min_supersampled_radius_squared < distance_squared && distance_squared <= max_supersampled_radius_squared) {
	  //if ((m_rad_max-1.)*(m_rad_max-1.)< distance_squared && distance_squared < m_rad_max * m_rad_max) {
	  //if ((m_rad_max*m_rad_max-thickness)< distance_squared && distance_squared < (m_rad_max * m_rad_max)) {
	  if (rad_min_squared < distance_squared && distance_squared < (m_rad_max * m_rad_max)) {
		  return 1.0;
	  }
	  return 0.;
  }

SeFloat EllipticalAperture::getRadiusSquared(SeFloat center_x, SeFloat center_y, SeFloat pixel_x,
                                             SeFloat pixel_y) const {
  auto dist_x = SeFloat(pixel_x) - center_x;
  auto dist_y = SeFloat(pixel_y) - center_y;

  return m_cxx * dist_x * dist_x + m_cyy * dist_y * dist_y + m_cxy * dist_x * dist_y;
}

PixelCoordinate EllipticalAperture::getMinPixel(SeFloat centroid_x, SeFloat centroid_y) const {
  SeFloat dx, dy;

  // compute the maximum extend in x/y
  dx = m_rad_max * std::sqrt(1.0 / (m_cxx - m_cxy * m_cxy / (4.0 * m_cyy)));
  dy = m_rad_max * std::sqrt(1.0 / (m_cyy - m_cxy * m_cxy / (4.0 * m_cxx)));

  // return the absolute values
  return PixelCoordinate(centroid_x - dx, centroid_y - dy);
}

PixelCoordinate EllipticalAperture::getMaxPixel(SeFloat centroid_x, SeFloat centroid_y) const {
  SeFloat dx, dy;

  // compute the maximum extend in x/y
  dx = m_rad_max * std::sqrt(1.0 / (m_cxx - m_cxy * m_cxy / (4.0 * m_cyy)));
  dy = m_rad_max * std::sqrt(1.0 / (m_cyy - m_cxy * m_cxy / (4.0 * m_cxx)));

  // return the absolute values
  return PixelCoordinate(centroid_x + dx + 1, centroid_y + dy + 1);
}

} // end SourceXtractor

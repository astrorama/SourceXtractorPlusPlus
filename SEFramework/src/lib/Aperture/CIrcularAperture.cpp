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
 * CircularAperture.cpp
 *
 *  Created on: Oct 08, 2018
 *      Author: Alejandro Alvarez
 */

#include "SEFramework/Aperture/CircularAperture.h"

namespace SourceXtractor {

// enhancing from 5 to 10 smoothens the photometry
const int SUPERSAMPLE_NB = 10;

SeFloat CircularAperture::getArea(SeFloat center_x, SeFloat center_y, SeFloat pixel_x, SeFloat pixel_y) const {
  auto dx = pixel_x - center_x;
  auto dy = pixel_y - center_y;
  SeFloat min_supersampled_radius_squared = m_radius > .75 ? (m_radius - .75) * (m_radius - .75) : 0;
  SeFloat max_supersampled_radius_squared = (m_radius + .75) * (m_radius + .75);

  auto distance_squared = dx * dx + dy * dy;
  SeFloat area = 0.0;
  if (distance_squared < min_supersampled_radius_squared) {
    area = 1.0;
  }
  else if (distance_squared <= max_supersampled_radius_squared) {
    for (int sub_y = 0; sub_y < SUPERSAMPLE_NB; sub_y++) {
      for (int sub_x = 0; sub_x < SUPERSAMPLE_NB; sub_x++) {
        auto dx2 = dx + SeFloat(sub_x - SUPERSAMPLE_NB / 2) / SUPERSAMPLE_NB;
        auto dy2 = dy + SeFloat(sub_y - SUPERSAMPLE_NB / 2) / SUPERSAMPLE_NB;
        auto supersampled_distance_squared = dx2 * dx2 + dy2 * dy2;
        if (supersampled_distance_squared <= m_radius * m_radius) {
          area += 1.0 / (SUPERSAMPLE_NB * SUPERSAMPLE_NB);
        }
      }
    }
  }
  return area;
}

SeFloat CircularAperture::getRadiusSquared(SeFloat center_x, SeFloat center_y, SeFloat pixel_x, SeFloat pixel_y) const {
  auto dist_x = SeFloat(pixel_x) - center_x;
  auto dist_y = SeFloat(pixel_y) - center_y;

  return dist_x * dist_x + dist_y * dist_y;
}

PixelCoordinate CircularAperture::getMinPixel(SeFloat centroid_x, SeFloat centroid_y) const {
  return PixelCoordinate(centroid_x - m_radius, centroid_y - m_radius);
}

PixelCoordinate CircularAperture::getMaxPixel(SeFloat centroid_x, SeFloat centroid_y) const {
  return PixelCoordinate(centroid_x + m_radius + 1, centroid_y + m_radius + 1);
}

} // end SourceXtractor

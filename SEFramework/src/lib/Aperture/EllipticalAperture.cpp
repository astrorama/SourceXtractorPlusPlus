/*
 * EllipticalAperture.cpp
 *
 *  Created on: Oct 08, 2018
 *      Author: Alejandro Alvarez
 */

#include "SEFramework/Aperture/EllipticalAperture.h"

namespace SExtractor {

// enhancing from 5 to 10 smoothens the photometry
const int SUPERSAMPLE_AUTO_NB = 10;


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

} // end SExtractor

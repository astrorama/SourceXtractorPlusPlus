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
 * FluxMeasurement.h
 *
 *  Created on: Oct 19, 2018
 *      Author: Alejandro Alvarez
 */

#ifndef _SEFRAMEWORK_SEFRAMEWORK_APERTURE_MEASUREFLUX_H
#define _SEFRAMEWORK_SEFRAMEWORK_APERTURE_MEASUREFLUX_H

#include "Aperture.h"
#include "SEFramework/Image/WriteableImage.h"
#include "SEFramework/Source/SourceFlags.h"

namespace SourceXtractor {

struct FluxMeasurement {
  SeFloat m_flux;
  SeFloat m_variance;
  SeFloat m_total_area, m_bad_area;
  Flags m_flags;

  FluxMeasurement() : m_flux(0.), m_variance(0.), m_total_area(0.), m_bad_area(0.), m_flags(Flags::NONE) {}
};

/**
 * Measure the flux on an image
 * @param aperture
 *  Aperture to use
 * @param centroid_x
 *  Center of the aperture on the X axis
 * @param centroid_y
 *  Center of the aperture on the Y axis
 * @param img
 *  The image where to measure
 * @param variance_map
 *  Variance map
 * @param variance_threshold
 *  If the pixel value in the variance map is greater than this value, the pixel will be ignored
 * @param use_symmetry
 *  If the pixel is ignored, try using the symmetric point value instead
 * @return
 *  A measurement with the total flux, variance, and the BOUNDARY flag if the aperture touches on a border
 */
FluxMeasurement measureFlux(const std::shared_ptr<Aperture> &aperture, SeFloat centroid_x, SeFloat centroid_y,
                            const std::shared_ptr<Image<SeFloat>> &img,
                            const std::shared_ptr<Image<SeFloat>> &variance_map, SeFloat variance_threshold,
                            bool use_symmetry);

/**
 * Fill the pixels that fall within the aperture with the given value. Useful for debugging.
 * @tparam T
 * @param aperture
 *  Aperture to use
 * @param centroid_x
 *  Center of the aperture on the X axis
 * @param centroid_y
 *  Center of the aperture on the Y axis
 * @param img
 *  Image to modify
 * @param value
 *  Value to use for the fill
 */
template <typename T>
void fillAperture(const std::shared_ptr<Aperture> &aperture, SeFloat centroid_x, SeFloat centroid_y,
                  const std::shared_ptr<WriteableImage<T>> &img, T value) {
  auto min_pixel = aperture->getMinPixel(centroid_x, centroid_y);
  auto max_pixel = aperture->getMaxPixel(centroid_x, centroid_y);

  for (int y = min_pixel.m_y; y <= max_pixel.m_y; ++y) {
    for (int x = min_pixel.m_x; x <= max_pixel.m_x; ++x) {
      if (aperture->getArea(centroid_x, centroid_y, x, y) > 0) {
        if (x >= 0 && y >= 0 && x < img->getWidth() && y < img->getHeight()) {
          img->setValue(x, y, value);
        }
      }
    }
  }
}

} // end SourceXtractor

#endif // _SEFRAMEWORK_SEFRAMEWORK_APERTURE_MEASUREFLUX_H

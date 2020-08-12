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
 * FluxMeasurement.cpp
 *
 *  Created on: Oct 19, 2018
 *      Author: Alejandro Alvarez
 */

#include "SEFramework/Aperture/FluxMeasurement.h"


namespace SourceXtractor {

const SeFloat BADAREA_THRESHOLD_APER = 0.1;

static std::tuple<SeFloat, SeFloat>
getMirrorPixel(SeFloat centroid_x, SeFloat centroid_y, int pixel_x, int pixel_y,
               const std::shared_ptr<Image<SeFloat>>& img,
               const std::shared_ptr<Image<SeFloat>>& variance_map,
               SeFloat variance_threshold) {
  // get the mirror pixel
  auto mirror_x = 2 * centroid_x - pixel_x + 0.49999;
  auto mirror_y = 2 * centroid_y - pixel_y + 0.49999;
  if (img->isInside(mirror_x, mirror_y)) {
    auto variance_tmp = variance_map ? variance_map->getValue(mirror_x, mirror_y) : 1;
    if (variance_tmp < variance_threshold) {
      // mirror pixel is OK: take the value
      return std::make_pair(img->getValue(mirror_x, mirror_y), variance_tmp);
    }
  }
  return std::make_pair(0., 0.);
}

FluxMeasurement measureFlux(const std::shared_ptr<Aperture> &aperture, SeFloat centroid_x, SeFloat centroid_y,
                            const std::shared_ptr<Image<SeFloat>> &img,
                            const std::shared_ptr<Image<SeFloat>> &variance_map, SeFloat variance_threshold,
                            bool use_symmetry) {
  auto min_pixel = aperture->getMinPixel(centroid_x, centroid_y);
  auto max_pixel = aperture->getMaxPixel(centroid_x, centroid_y);

  FluxMeasurement measurement;

  // Skip if the full source is outside the frame
  if (max_pixel.m_x < 0 || max_pixel.m_y < 0 || min_pixel.m_x >= img->getWidth() ||
      min_pixel.m_y >= img->getHeight()) {
    measurement.m_flags = Flags::OUTSIDE;
    return measurement;
  }

  // iterate over the aperture pixels
  for (int pixel_y = min_pixel.m_y; pixel_y <= max_pixel.m_y; pixel_y++) {
    for (int pixel_x = min_pixel.m_x; pixel_x <= max_pixel.m_x; pixel_x++) {
      SeFloat pixel_value = 0;
      SeFloat pixel_variance = 0;

      // get the area coverage and continue if there is overlap
      auto area = aperture->getArea(centroid_x, centroid_y, pixel_x, pixel_y);
      if (area == 0) {
        continue;
      }

      measurement.m_total_area += area;

      // make sure the pixel is inside the image
      if (img->isInside(pixel_x, pixel_y)) {

        SeFloat variance_tmp = variance_map ? variance_map->getValue(pixel_x, pixel_y) : 1;
        if (variance_tmp > variance_threshold) {
          measurement.m_bad_area += 1;
          if (use_symmetry) {
            std::tie(pixel_value, pixel_variance) = getMirrorPixel(
              centroid_x, centroid_y, pixel_x, pixel_y, img, variance_map, variance_threshold);
          }
        }
        else {
          pixel_value = img->getValue(pixel_x, pixel_y);
          pixel_variance = variance_tmp;
        }

        measurement.m_flux += pixel_value * area;
        measurement.m_variance += pixel_variance * area;
      } else {
        measurement.m_flags |= Flags::BOUNDARY;
      }
    }
  }

  // check/set the bad area flag
  if (measurement.m_total_area > 0 &&
      measurement.m_bad_area / measurement.m_total_area > BADAREA_THRESHOLD_APER) {
    measurement.m_flags |= Flags::BIASED;
  }

  return measurement;
}

} // end SourceXtractor

/*
 * FluxMeasurement.cpp
 *
 *  Created on: Oct 19, 2018
 *      Author: Alejandro Alvarez
 */

#include "SEFramework/Aperture/FluxMeasurement.h"


namespace SExtractor {

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
    return measurement;
  }

  // iterate over the aperture pixels
  for (int pixel_y = min_pixel.m_y; pixel_y <= max_pixel.m_y; pixel_y++) {
    for (int pixel_x = min_pixel.m_x; pixel_x <= max_pixel.m_x; pixel_x++) {
      SeFloat pixel_value = 0;
      SeFloat pixel_variance = 0;
      SeFloat variance_tmp = 0;

      // get the area coverage and continue if there is overlap
      auto area = aperture->getArea(centroid_x, centroid_y, pixel_x, pixel_y);
      if (area > 0) {

        // make sure the pixel is inside the image
        if (pixel_x >= 0 && pixel_y >= 0 && pixel_x < img->getWidth() && pixel_y < img->getHeight()) {

          variance_tmp = variance_map ? variance_map->getValue(pixel_x, pixel_y) : 1;
          if (variance_tmp > variance_threshold) {

            // TODO: Should we flag bad pixel?

            // try using the mirror pixel
            if (use_symmetry) {
              // get the mirror pixel
              auto mirror_x = 2 * centroid_x - pixel_x + 0.49999;
              auto mirror_y = 2 * centroid_y - pixel_y + 0.49999;
              if (mirror_x >= 0 && mirror_y >= 0 && mirror_x < img->getWidth() && mirror_y < img->getHeight()) {
                variance_tmp = variance_map ? variance_map->getValue(mirror_x, mirror_y) : 1;
                if (variance_tmp < variance_threshold) {
                  // mirror pixel is OK: take the value
                  pixel_value = img->getValue(mirror_x, mirror_y);
                  pixel_variance = variance_tmp;
                }
              }
            }
          } else {
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
  }

  return measurement;
}

} // end SExtractor

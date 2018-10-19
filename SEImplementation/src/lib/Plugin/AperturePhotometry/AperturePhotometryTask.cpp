/*
 * AperturePhotometryTask.cpp
 *
 *  Created on: Sep 22, 2016
 *      Author: mschefer
 */

#include <iostream>

#include "SEFramework/Aperture/TransformedAperture.h"
#include "SEFramework/Source/SourceFlags.h"
#include "SEImplementation/Plugin/AperturePhotometry/AperturePhotometryTask.h"
#include "SEImplementation/Plugin/MeasurementFrame/MeasurementFrame.h"
#include "SEImplementation/Plugin/MeasurementFramePixelCentroid/MeasurementFramePixelCentroid.h"
#include "SEImplementation/Plugin/Jacobian/Jacobian.h"
#include "SEImplementation/Plugin/AperturePhotometry/AperturePhotometry.h"
#include "SEImplementation/CheckImages/CheckImages.h"
#include "SEImplementation/Plugin/SourceIDs/SourceID.h"
#include "SEImplementation/Plugin/AperturePhotometry/ApertureFlag.h"
#include "SEFramework/Aperture/CircularAperture.h"


namespace SExtractor {

namespace {
const SeFloat CROWD_THRESHOLD_APER = 0.1;
const SeFloat BADAREA_THRESHOLD_APER = 0.1;
}

//////////////////////////////////////////////////////////////////////////////////////////

void AperturePhotometryTask::computeProperties(SourceInterface &source) const {
  auto measurement_frame = source.getProperty<MeasurementFrame>(m_instance).getFrame();
  auto measurement_image = measurement_frame->getSubtractedImage();
  auto variance_map = measurement_frame->getVarianceMap();
  auto variance_threshold = measurement_frame->getVarianceThreshold();
  auto pixel_centroid = source.getProperty<MeasurementFramePixelCentroid>(m_instance);

  // get the object center
  const auto &centroid_x = source.getProperty<MeasurementFramePixelCentroid>(m_instance).getCentroidX();
  const auto &centroid_y = source.getProperty<MeasurementFramePixelCentroid>(m_instance).getCentroidY();

  // m_apertures is the aperture on the detection frame, so we have to wrap it
  // to transform it to the measurement frame
  auto jacobian = source.getProperty<JacobianSource>(m_instance);

  std::vector<SeFloat> fluxes, fluxes_error;
  std::vector<SeFloat> mags, mags_error;
  std::vector<Flags> flags;

  for (auto aperture_size : m_apertures) {
    auto aperture = TransformedAperture(std::make_shared<CircularAperture>(aperture_size), jacobian.asTuple());

    // get the aperture borders on the image
    auto min_pixel = aperture.getMinPixel(centroid_x, centroid_y);
    auto max_pixel = aperture.getMaxPixel(centroid_x, centroid_y);

    SeFloat total_flux = 0;
    SeFloat total_variance = 0.0;
    Flags flag = Flags::NONE;

    // Skip if the full source is outside the frame
    if (max_pixel.m_x < 0 || max_pixel.m_y < 0 || min_pixel.m_x >= measurement_image->getWidth() ||
        min_pixel.m_y >= measurement_image->getHeight()) {
      fluxes.push_back(0.);
      fluxes_error.push_back(0.);
      mags.push_back(99.);
      mags_error.push_back(99.);
      flags.push_back(flag);
      continue;
    }

    // iterate over the aperture pixels
    for (int pixel_y = min_pixel.m_y; pixel_y <= max_pixel.m_y; pixel_y++) {
      for (int pixel_x = min_pixel.m_x; pixel_x <= max_pixel.m_x; pixel_x++) {
        SeFloat pixel_value = 0;
        SeFloat pixel_variance = 0;
        SeFloat variance_tmp = 0;

        // get the area coverage and continue if there is overlap
        auto area = aperture.getArea(centroid_x, centroid_y, pixel_x, pixel_y);
        if (area > 0) {

          // make sure the pixel is inside the image
          if (pixel_x >= 0 && pixel_y >= 0 && pixel_x < measurement_image->getWidth() &&
              pixel_y < measurement_image->getHeight()) {

            variance_tmp = variance_map ? variance_map->getValue(pixel_x, pixel_y) : 1;
            if (variance_tmp > variance_threshold) {
              // try using the mirror pixel
              if (m_use_symmetry) {
                // get the mirror pixel
                auto mirror_x = 2 * centroid_x - pixel_x + 0.49999;
                auto mirror_y = 2 * centroid_y - pixel_y + 0.49999;
                if (mirror_x >= 0 && mirror_y >= 0 && mirror_x < measurement_image->getWidth() &&
                    mirror_y < measurement_image->getHeight()) {
                  variance_tmp = variance_map ? variance_map->getValue(mirror_x, mirror_y) : 1;
                  if (variance_tmp < variance_threshold) {
                    // mirror pixel is OK: take the value
                    pixel_value = measurement_image->getValue(mirror_x, mirror_y);
                    pixel_variance = variance_tmp;
                  }
                }
              }
            } else {
              pixel_value = measurement_image->getValue(pixel_x, pixel_y);
              pixel_variance = variance_tmp;
            }
            total_flux += pixel_value * area;
            total_variance += pixel_variance * area;
          } else {
            flag |= Flags::BOUNDARY;
          }
        }
      }
    }

    // compute the derived quantities
    auto flux_error = sqrt(total_variance);
    auto mag = total_flux > 0.0 ? -2.5 * log10(total_flux) + m_magnitude_zero_point : SeFloat(99.0);
    auto mag_error = 1.0857 * flux_error / total_flux;

    fluxes.push_back(total_flux);
    fluxes_error.push_back(flux_error);
    mags.push_back(mag);
    mags_error.push_back(mag_error);
    flags.push_back(flag);
  }

  // Merge flags with those set on the detection frame
  auto aperture_flags = source.getProperty<ApertureFlag>().getFlags();

  for (size_t i = 0; i < aperture_flags.size(); ++i) {
    flags[i] |= aperture_flags[i];
  }

  // set the source properties
  source.setIndexedProperty<AperturePhotometry>(m_instance, fluxes, fluxes_error, mags, mags_error, flags);

  // Draw the last aperture
  auto aperture = TransformedAperture(std::make_shared<CircularAperture>(m_apertures[0]), jacobian.asTuple());
  auto coord_system = measurement_frame->getCoordinateSystem();
  auto aperture_check_img = CheckImages::getInstance().getApertureImage(m_instance, measurement_image->getWidth(),
                                                                        measurement_image->getHeight(), coord_system);

  auto min_pixel = aperture.getMinPixel(centroid_x, centroid_y);
  auto max_pixel = aperture.getMaxPixel(centroid_x, centroid_y);

  if (aperture_check_img) {
    auto src_id = source.getProperty<SourceID>().getId();

    for (int y = min_pixel.m_y; y <= max_pixel.m_y; ++y) {
      for (int x = min_pixel.m_x; x <= max_pixel.m_x; ++x) {
        if (aperture.getArea(centroid_x, centroid_y, x, y) > 0) {
          if (x >= 0 && y >= 0 && x < aperture_check_img->getWidth() && y < aperture_check_img->getHeight()) {
            aperture_check_img->setValue(x, y, src_id);
          }
        }
      }
    }
  }
}

}


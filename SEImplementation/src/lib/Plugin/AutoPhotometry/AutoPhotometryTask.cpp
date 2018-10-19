/*
 * AutoPhotometryTask.cpp
 *
 *  Created on: Jul 18, 2016
 *      Author: mkuemmel@usm.lmu.de
 */
//#include <math.h>
#include <iostream>
#include "SEImplementation/Plugin/MeasurementFrame/MeasurementFrame.h"
#include "SEImplementation/Plugin/MeasurementFramePixelCentroid/MeasurementFramePixelCentroid.h"
#include "SEImplementation/Plugin/ShapeParameters/ShapeParameters.h"
#include "SEImplementation/Plugin/KronRadius/KronRadius.h"
#include "SEImplementation/Plugin/Jacobian/Jacobian.h"
#include "SEImplementation/CheckImages/CheckImages.h"
#include "SEImplementation/Plugin/SourceIDs/SourceID.h"
#include "SEFramework/Aperture/EllipticalAperture.h"
#include "SEFramework/Aperture/TransformedAperture.h"
#include "SEFramework/Source/SourceFlags.h"
#include "SEImplementation/Plugin/AutoPhotometry/AutoPhotometry.h"
#include "SEImplementation/Plugin/AutoPhotometry/AutoPhotometryTask.h"


namespace SExtractor {

namespace {
const SeFloat CROWD_THRESHOLD_AUTO = 0.1;
const SeFloat BADAREA_THRESHOLD_AUTO = 0.1;
}

//////////////////////////////////////////////////////////////////////////////////////////

void AutoPhotometryTask::computeProperties(SourceInterface &source) const {
  // get the measurement frame
  const auto &measurement_frame = source.getProperty<MeasurementFrame>(m_instance).getFrame();

  // get the images and image information from the frame
  const auto &measurement_image = measurement_frame->getSubtractedImage();
  const auto &variance_map = measurement_frame->getVarianceMap();
  const auto &variance_threshold = measurement_frame->getVarianceThreshold();
  const auto &threshold_image = measurement_frame->getThresholdedImage();

  // get the object center
  const auto &centroid_x = source.getProperty<MeasurementFramePixelCentroid>(m_instance).getCentroidX();
  const auto &centroid_y = source.getProperty<MeasurementFramePixelCentroid>(m_instance).getCentroidY();

  // get the shape parameters
  const auto &cxx = source.getProperty<ShapeParameters>().getEllipseCxx();
  const auto &cyy = source.getProperty<ShapeParameters>().getEllipseCyy();
  const auto &cxy = source.getProperty<ShapeParameters>().getEllipseCxy();

  // get the jacobian
  const auto &jacobian = source.getProperty<JacobianSource>(m_instance);

  // get the kron-radius
  SeFloat kron_radius_auto = m_kron_factor * source.getProperty<KronRadius>().getKronRadius();
  if (kron_radius_auto < m_kron_minrad)
    kron_radius_auto = m_kron_minrad;

  // create the elliptical aperture
  auto ell_aper = TransformedAperture(std::make_shared<EllipticalAperture>(cxx, cyy, cxy, kron_radius_auto),
                                      jacobian.asTuple());

  // get the aperture borders on the image
  const auto &min_pixel = ell_aper.getMinPixel(centroid_x, centroid_y);
  const auto &max_pixel = ell_aper.getMaxPixel(centroid_x, centroid_y);

  SeFloat total_flux = 0;
  SeFloat total_variance = 0.0;
  Flags flag = Flags::NONE;

  // Skip if the full source is outside the frame
  if (max_pixel.m_x < 0 || max_pixel.m_y < 0 || min_pixel.m_x >= measurement_image->getWidth() ||
      min_pixel.m_y >= measurement_image->getHeight()) {
    source.setIndexedProperty<AutoPhotometry>(m_instance, 0., 0., 99., 99., flag);
    return;
  }

  // iterate over the aperture pixels
  for (int pixel_y = min_pixel.m_y; pixel_y <= max_pixel.m_y; pixel_y++) {
    for (int pixel_x = min_pixel.m_x; pixel_x <= max_pixel.m_x; pixel_x++) {
      SeFloat pixel_value = 0;
      SeFloat pixel_variance = 0;
      SeFloat variance_tmp = 0;

      // check whether the pixel is in the ellipse
      if (ell_aper.getArea(centroid_x, centroid_y, pixel_x, pixel_y) > 0) {

        // check whether the pixel is inside the image
        if (pixel_x >= 0 && pixel_y >= 0 && pixel_x < measurement_image->getWidth() &&
            pixel_y < measurement_image->getHeight()) {

          // check whether the pixel is OK
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
            // pixel is OK: take the value
            pixel_value = measurement_image->getValue(pixel_x, pixel_y);
            pixel_variance = variance_tmp;
          }
          total_flux += pixel_value;
          total_variance += pixel_variance;
        }
        else {
          flag |= Flags::BOUNDARY;
        }
      }
    }
  }

  // compute the derived quantities
  auto flux_error = sqrt(total_variance);
  auto mag = total_flux > 0.0 ? -2.5 * log10(total_flux) + m_magnitude_zero_point : SeFloat(99.0);
  auto mag_error = 1.0857 * flux_error / total_flux;

  // set the source properties
  source.setIndexedProperty<AutoPhotometry>(m_instance, total_flux, flux_error, mag, mag_error, flag);

  // Draw the aperture
  auto coord_system = measurement_frame->getCoordinateSystem();
  auto aperture_check_img = CheckImages::getInstance().getAutoApertureImage(m_instance, measurement_image->getWidth(),
                                                                            measurement_image->getHeight(),
                                                                            coord_system);
  if (aperture_check_img) {
    auto src_id = source.getProperty<SourceID>().getId();

    for (int y = min_pixel.m_y; y <= max_pixel.m_y; ++y) {
      for (int x = min_pixel.m_x; x <= max_pixel.m_x; ++x) {
        if (ell_aper.getArea(centroid_x, centroid_y, x, y) > 0) {
          if (x >= 0 && y >= 0 && x < aperture_check_img->getWidth() && y < aperture_check_img->getHeight()) {
            aperture_check_img->setValue(x, y, src_id);
          }
        }
      }
    }
  }
}

}


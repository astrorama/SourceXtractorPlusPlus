/*
 * AperturePhotometryTask.cpp
 *
 *  Created on: Sep 22, 2016
 *      Author: mschefer
 */

#include <iostream>

#include "SEFramework/Property/DetectionFrame.h"
#include "SEImplementation/Plugin/PixelCentroid/PixelCentroid.h"

#include "SEImplementation/Plugin/AperturePhotometry/AperturePhotometry.h"
#include "SEImplementation/Plugin/AperturePhotometry/AperturePhotometryTask.h"

namespace SExtractor {

namespace {
  // enhancing from 5 to 10 smoothens the photometry
  const int SUPERSAMPLE_NB = 10;
}

//////////////////////////////////////////////////////////////////////////////////////////

void AperturePhotometryTask::computeProperties(SourceInterface& source) const {
  // get the detection frame
  const auto& detection_frame  = source.getProperty<DetectionFrame>().getFrame();

  // get the images and image information from the frame
  const auto& detection_image    = detection_frame->getSubtractedImage();
  const auto& detection_variance = detection_frame->getVarianceMap();
  const auto& variance_threshold = detection_frame->getVarianceThreshold();

  // get the object center
  const auto& centroid_x = source.getProperty<PixelCentroid>().getCentroidX();
  const auto& centroid_y = source.getProperty<PixelCentroid>().getCentroidY();

  // get the aperture borders on the image
  auto min_pixel = m_aperture->getMinPixel(centroid_x, centroid_y);
  auto max_pixel = m_aperture->getMaxPixel(centroid_x, centroid_y);

  SeFloat total_flux     = 0;
  SeFloat total_variance = 0.0;
  //SeFloat total_area = 0.0;

  // iterate over the aperture pixels
  for (int pixel_y = min_pixel.m_y; pixel_y <= max_pixel.m_y; pixel_y++) {
    for (int pixel_x = min_pixel.m_x; pixel_x <= max_pixel.m_x; pixel_x++) {
      SeFloat value = 0;
      SeFloat pixel_variance = 0;

      // make sure the pixel is inside the image
      if (pixel_x >=0 && pixel_y >=0 && pixel_x < detection_image->getWidth() && pixel_y < detection_image->getHeight()) {

        // get the variance value
        pixel_variance = detection_variance ? detection_variance->getValue(pixel_x, pixel_y) : 1;

        // check whether the pixel is "good"
        if (pixel_variance < variance_threshold) {
          value = detection_image->getValue(pixel_x, pixel_y);
        }
        else if (m_use_symmetry) {

          // for bad pixels get the mirror pixel
          auto mirror_x = 2 * centroid_x - pixel_x + 0.49999;
          auto mirror_y = 2 * centroid_y - pixel_y + 0.49999;
          if (mirror_x >=0 && mirror_y >=0 && mirror_x < detection_image->getWidth() && mirror_y < detection_image->getHeight()) {
            pixel_variance = detection_variance ? detection_variance->getValue(mirror_x, mirror_y) : 1;

            // check whether the mirror pixel is good
            if (pixel_variance < variance_threshold) {
              value = detection_image->getValue(mirror_x, mirror_y);
            } else {
              // set bad pixel values to zero
              value=0.0;
              pixel_variance=0.0;
            }
          }
        } else {
          // set bad pixel values to zero
          value=0.0;
          pixel_variance=0.0;
        }
        // get the area and enhance the flux
        auto area = m_aperture->getArea(centroid_x, centroid_y, pixel_x, pixel_y);
        total_flux     += value * area;
        total_variance +=  pixel_variance * area;
      }
    }
  }

  // compute the derived quantities
  auto flux_error = sqrt(total_variance);
  auto mag = total_flux > 0.0 ? -2.5*log10(total_flux) + m_magnitude_zero_point : SeFloat(99.0);
  auto mag_error = 1.0857 * flux_error / total_flux;

  // set the source properties
  source.setIndexedProperty<AperturePhotometry>(m_instance, total_flux, flux_error, mag, mag_error);
}

//////////////////////////////////////////////////////////////////////////////////////////

void AperturePhotometryAggregateTask::computeProperties(SourceInterface& source) const {
  SeFloat flux = 0;
  for (auto instance : m_instances_to_aggregate) {
    auto aperture_photometry = source.getProperty<AperturePhotometry>(instance);
    flux += aperture_photometry.getFlux();
  }
  flux /= m_instances_to_aggregate.size();

  auto mag = flux > 0.0 ? -2.5*log10(flux) + m_magnitude_zero_point : SeFloat(99.0);
  source.setIndexedProperty<AperturePhotometry>(m_instance, flux, 999999, mag, 999999); // FIXME
}

//////////////////////////////////////////////////////////////////////////////////////////

SeFloat CircularAperture::getArea(SeFloat center_x, SeFloat center_y, int pixel_x, int pixel_y) const {
  auto dx = pixel_x - center_x;
  auto dy = pixel_y - center_y;
  SeFloat min_supersampled_radius_squared = m_radius > .75 ? (m_radius - .75) * (m_radius - .75) : 0;
  SeFloat max_supersampled_radius_squared = (m_radius + .75) * (m_radius + .75);

  auto distance_squared = dx * dx + dy * dy;
  if (distance_squared <= max_supersampled_radius_squared) {
    if (distance_squared < min_supersampled_radius_squared) {
      return 1.0;
    } else {
      SeFloat area = 0.0;
      for (int sub_y = 0; sub_y < SUPERSAMPLE_NB; sub_y++) {
        for (int sub_x = 0; sub_x < SUPERSAMPLE_NB; sub_x++) {
          auto dx2 = dx + SeFloat(sub_x - SUPERSAMPLE_NB/2) / SUPERSAMPLE_NB;
          auto dy2 = dy + SeFloat(sub_y - SUPERSAMPLE_NB/2) / SUPERSAMPLE_NB;
          auto supersampled_distance_squared = dx2 * dx2 + dy2 * dy2;
          if (supersampled_distance_squared <= m_radius * m_radius) {
            area += 1.0 / (SUPERSAMPLE_NB * SUPERSAMPLE_NB);
          }
        }
      }
      return area;
    }
  }
  return 0.0;
}

PixelCoordinate CircularAperture::getMinPixel(SeFloat centroid_x, SeFloat centroid_y) const {
  return PixelCoordinate(centroid_x - m_radius, centroid_y - m_radius);
}

PixelCoordinate CircularAperture::getMaxPixel(SeFloat centroid_x, SeFloat centroid_y) const {
  return PixelCoordinate(centroid_x + m_radius + 1, centroid_y + m_radius + 1);
}

}


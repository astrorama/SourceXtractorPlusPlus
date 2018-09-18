/*
 * AperturePhotometryTask.cpp
 *
 *  Created on: Sep 22, 2016
 *      Author: mschefer
 */

#include <iostream>

#include "SEImplementation/Plugin/MeasurementFrame/MeasurementFrame.h"
#include "SEImplementation/Plugin/MeasurementFramePixelCentroid/MeasurementFramePixelCentroid.h"

#include "SEImplementation/Plugin/AperturePhotometry/AperturePhotometry.h"
#include "SEImplementation/Plugin/AperturePhotometry/AperturePhotometryTask.h"

namespace SExtractor {

namespace {
  // enhancing from 5 to 10 smoothens the photometry
  const int SUPERSAMPLE_NB = 10;
}

//////////////////////////////////////////////////////////////////////////////////////////

void AperturePhotometryTask::computeProperties(SourceInterface& source) const {
  auto measurement_frame = source.getProperty<MeasurementFrame>(m_image_instance).getFrame();

  auto measurement_image = measurement_frame->getSubtractedImage();
  auto variance_map = measurement_frame->getVarianceMap();
  auto variance_threshold = measurement_frame->getVarianceThreshold();

  auto pixel_centroid = source.getProperty<MeasurementFramePixelCentroid>(m_image_instance);

  auto min_pixel = m_aperture->getMinPixel(pixel_centroid.getCentroidX(), pixel_centroid.getCentroidY());
  auto max_pixel = m_aperture->getMaxPixel(pixel_centroid.getCentroidX(), pixel_centroid.getCentroidY());

  SeFloat total_flux = 0;
  SeFloat total_variance = 0.0;
  long int flag = 0;

  // iterate over the aperture pixels
  for (int pixel_y = min_pixel.m_y; pixel_y <= max_pixel.m_y; pixel_y++) {
    for (int pixel_x = min_pixel.m_x; pixel_x <= max_pixel.m_x; pixel_x++) {
      MeasurementImage::PixelType value = 0;
      WeightImage::PixelType variance = 0;

      if (pixel_x >=0 && pixel_y >=0 && pixel_x < measurement_image->getWidth() && pixel_y < measurement_image->getHeight()) {

        auto pixel_variance = variance_map ? variance_map->getValue(pixel_x, pixel_y) : 1;
        if (pixel_variance < variance_threshold) {
          value = measurement_image->getValue(pixel_x, pixel_y);
          variance = pixel_variance;
        } else if (m_use_symmetry) {
          auto mirror_x = 2 * pixel_centroid.getCentroidX() - pixel_x + 0.49999;
          auto mirror_y = 2 * pixel_centroid.getCentroidY() - pixel_y + 0.49999;
          if (mirror_x >=0 && mirror_y >=0 && mirror_x < measurement_image->getWidth() && mirror_y < measurement_image->getHeight()) {
            auto mirror_pixel_variance = variance_map ? variance_map->getValue(mirror_x, mirror_y) : 1;
            if (mirror_pixel_variance < variance_threshold) {
              value = measurement_image->getValue(mirror_x, mirror_y);
              variance = mirror_pixel_variance;
            }
          }
        }
        else {
          flag |= 0x0008;
        }
      }

      auto area = m_aperture->getArea(pixel_centroid.getCentroidX(), pixel_centroid.getCentroidY(), pixel_x, pixel_y);

      // FIXME add gain noise, weight_gain flag support?
//      if (gain > 0 && total_flux > 0) {
//        total_variance += total_flux / gain;
//      }

      total_flux += value * area;
      total_variance +=  variance * area;
    }
  }

  // compute the derived quantities
  auto flux_error = sqrt(total_variance);
  auto mag = total_flux > 0.0 ? -2.5*log10(total_flux) + m_magnitude_zero_point : SeFloat(99.0);
  auto mag_error = 1.0857 * flux_error / total_flux;

  // set the source properties
  source.setIndexedProperty<AperturePhotometry>(m_instance, total_flux, flux_error, mag, mag_error, flag);
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
  source.setIndexedProperty<AperturePhotometry>(m_instance, flux, 999999, mag, 999999, 0); // FIXME
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


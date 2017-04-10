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
  const int SUPERSAMPLE_NB = 5;
}

//////////////////////////////////////////////////////////////////////////////////////////

void AperturePhotometryTask::computeProperties(SourceInterface& source) const {
  auto measurement_frame = source.getProperty<MeasurementFrame>(m_image_instance).getFrame();
  auto measurement_image = measurement_frame->getSubtractedImage();
  auto weight_image = source.getProperty<MeasurementFrame>(m_image_instance).getFrame()->getWeightImage();

  //const auto& detection_frame = source.getProperty<DetectionFrame>();
  //SeFloat background_variance = detection_frame.getBackgroundRMS() * detection_frame.getBackgroundRMS();

  auto pixel_centroid = source.getProperty<MeasurementFramePixelCentroid>(m_image_instance);

  auto min_pixel = m_aperture->getMinPixel(pixel_centroid.getCentroidX(), pixel_centroid.getCentroidY());
  auto max_pixel = m_aperture->getMaxPixel(pixel_centroid.getCentroidX(), pixel_centroid.getCentroidY());

  SeFloat total_flux = 0;
  SeFloat total_variance = 0.0;

  auto rms = measurement_frame->getBackgroundRMS();
  SeFloat background_variance = (weight_image != nullptr && measurement_frame->isWeightAbsolute()) ? 1 : rms * rms;

  for (int pixel_y = min_pixel.m_y; pixel_y <= max_pixel.m_y; pixel_y++) {
    for (int pixel_x = min_pixel.m_x; pixel_x <= max_pixel.m_x; pixel_x++) {
      MeasurementImage::PixelType value = 0;
      WeightImage::PixelType pixel_variance = 0;

      if (pixel_x >=0 && pixel_y >=0 && pixel_x < measurement_image->getWidth() && pixel_y < measurement_image->getHeight()) {
        value = measurement_image->getValue(pixel_x, pixel_y);
        pixel_variance = weight_image ? weight_image->getValue(pixel_x, pixel_y) : 1;
      } else if (m_use_symmetry) {
        auto mirror_x = 2 * pixel_centroid.getCentroidX() - pixel_x + 0.49999;
        auto mirror_y = 2 * pixel_centroid.getCentroidY() - pixel_y + 0.49999;
        if (mirror_x >=0 && mirror_y >=0 && mirror_x < measurement_image->getWidth() && mirror_y < measurement_image->getHeight()) {
          value = measurement_image->getValue(mirror_x, mirror_y);
          pixel_variance = weight_image ? weight_image->getValue(mirror_x, mirror_y) : 1;
        }
      }

      auto area = m_aperture->getArea(pixel_centroid.getCentroidX(), pixel_centroid.getCentroidY(), pixel_x, pixel_y);

      // FIXME add gain noise
//      if (gainflag && pix>0.0 && gain>0.0)
//        sigtv += pix/gain*var/backnoise2;
//      }

      total_flux += value * area;
      total_variance +=  pixel_variance * background_variance * area;
    }
  }

  auto flux_error = sqrt(total_variance);

  auto mag = total_flux > 0.0 ? -2.5*log10(total_flux) + m_magnitude_zero_point : SeFloat(99.0);
  auto mag_error = 1.0857 * flux_error / total_flux;

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
  source.setIndexedProperty<AperturePhotometry>(m_instance, flux, 999999, mag, 999999);
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


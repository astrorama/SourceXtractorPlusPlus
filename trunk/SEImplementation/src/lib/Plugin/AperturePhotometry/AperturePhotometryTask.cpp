/*
 * AperturePhotometryTask.cpp
 *
 *  Created on: Sep 22, 2016
 *      Author: mschefer
 */

#include "SEImplementation/Plugin/MeasurementFrame/MeasurementFrame.h"
#include "SEImplementation/Plugin/MeasurementFramePixelCentroid/MeasurementFramePixelCentroid.h"

#include "SEImplementation/Plugin/AperturePhotometry/AperturePhotometry.h"
#include "SEImplementation/Plugin/AperturePhotometry/AperturePhotometryTask.h"

namespace SExtractor {

namespace {
  const int SUPERSAMPLE_NB = 5;
}

void AperturePhotometryTask::computeProperties(SourceInterface& source) const {
  auto measurement_frame = source.getProperty<MeasurementFrame>(m_image_instance).getMeasurementImage();
  auto pixel_centroid = source.getProperty<MeasurementFramePixelCentroid>(m_image_instance);

  auto min_pixel = m_aperture->getMinPixel(pixel_centroid.getCentroidX(), pixel_centroid.getCentroidY());
  auto min_x = std::max<int>(0, min_pixel.m_x);
  auto min_y = std::max<int>(0, min_pixel.m_y);

  auto max_pixel = m_aperture->getMaxPixel(pixel_centroid.getCentroidX(), pixel_centroid.getCentroidY());
  auto max_x = std::min<int>(measurement_frame->getWidth() - 1, max_pixel.m_x);
  auto max_y = std::min<int>(measurement_frame->getHeight() - 1, max_pixel.m_y);

  SeFloat flux = 0;
  for (int pixel_y = min_y; pixel_y <= max_y; pixel_y++) {
    for (int pixel_x = min_x; pixel_x <= max_x; pixel_x++) {
      auto value = measurement_frame->getValue(pixel_x, pixel_y);
      flux += value * m_aperture->getArea(pixel_centroid.getCentroidX(),
          pixel_centroid.getCentroidY(), pixel_x, pixel_y);
    }
  }

  auto mag = flux > 0.0 ? -2.5*log10(flux) + m_magnitude_zero_point : SeFloat(99.0);
  source.setIndexedProperty<AperturePhotometry>(m_instance, flux, mag);
}

void AperturePhotometryAggregateTask::computeProperties(SourceInterface& source) const {
  SeFloat flux;
  for (auto instance : m_instances_to_aggregate) {
    auto aperture_photometry = source.getProperty<AperturePhotometry>(instance);
    flux += aperture_photometry.getFlux();
  }
  flux /= m_instances_to_aggregate.size();

  auto mag = flux > 0.0 ? -2.5*log10(flux) + m_magnitude_zero_point : SeFloat(99.0);
  source.setIndexedProperty<AperturePhotometry>(m_instance, flux, mag);
}

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


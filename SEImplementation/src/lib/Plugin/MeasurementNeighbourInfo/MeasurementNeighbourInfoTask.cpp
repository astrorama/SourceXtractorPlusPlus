/*
 * MeasurementNeighbourInfoTask.cpp
 *
 *  Created on: Sep 28, 2018
 *      Author: Alejandro Alvarez Ayllon
 */

#include "SEImplementation/Plugin/PixelBoundaries/PixelBoundaries.h"
#include "SEFramework/Property/DetectionFrame.h"
#include "SEImplementation/Plugin/DetectionNeighbourInfo/DetectionNeighbourInfo.h"
#include "SEImplementation/Plugin/MeasurementFrameRectangle/MeasurementFrameRectangle.h"
#include "SEImplementation/Plugin/MeasurementFrame/MeasurementFrame.h"
#include "SEImplementation/Plugin/MeasurementNeighbourInfo/MeasurementNeighbourInfoTask.h"
#include "SEImplementation/Plugin/MeasurementNeighbourInfo/MeasurementNeighbourInfo.h"

namespace SExtractor {

MeasurementNeighbourInfoTask::MeasurementNeighbourInfoTask(unsigned instance) : m_instance{instance} {}

void MeasurementNeighbourInfoTask::computeProperties(SExtractor::SourceInterface &source) const {
  const auto &measurement_frame = source.getProperty<MeasurementFrame>(m_instance).getFrame();
  const auto &measurement_boundaries = source.getProperty<MeasurementFrameRectangle>(m_instance);
  const auto &detection_frame = source.getProperty<DetectionFrame>().getFrame();
  const auto &detection_boundaries = source.getProperty<PixelBoundaries>();

  const auto &measurement_coordinates = measurement_frame->getCoordinateSystem();

  const auto &detection_coordinates = detection_frame->getCoordinateSystem();
  const auto &detection_pixel_min = detection_boundaries.getMin();

  // If the source is outside bounds, skip
  if (measurement_boundaries.getWidth() <= 0) {
    source.setIndexedProperty<MeasurementNeighbourInfo>(m_instance);
    return;
  }

  const auto &detection_neighbour = source.getProperty<DetectionNeighbourInfo>();
  const auto &detection_neighbour_img = detection_neighbour.getImage();

  // Project this image to the measurement frame
  // Naive "nearest" pixel algorithm may probably be enough, as it doesn't make (?)
  // much sense to interpolate a flag
  auto measurement_pixel_min = measurement_boundaries.getTopLeft();

  auto measurement_neighbour_img = VectorImage<int>::create(measurement_boundaries.getWidth(),
                                                            measurement_boundaries.getHeight());

  for (int my = 0; my < measurement_neighbour_img->getHeight(); ++my) {
    for (int mx = 0; mx < measurement_neighbour_img->getWidth(); ++mx) {
      int m_abs_y = my + measurement_pixel_min.m_y;
      int m_abs_x = mx + measurement_pixel_min.m_x;

      auto dcoords = detection_coordinates->worldToImage(
        measurement_coordinates->imageToWorld(ImageCoordinate(m_abs_x, m_abs_y))
      );

      int detection_y = round(dcoords.m_y);
      int detection_x = round(dcoords.m_x);

      int dy = detection_y - detection_pixel_min.m_y;
      int dx = detection_x - detection_pixel_min.m_x;

      dy = std::max(0, std::min(dy, detection_neighbour_img->getHeight() - 1));
      dx = std::max(0, std::min(dx, detection_neighbour_img->getWidth() - 1));

      measurement_neighbour_img->setValue(mx, my, detection_neighbour_img->getValue(dx, dy));
    }
  }

  // Store as a property
  source.setIndexedProperty<MeasurementNeighbourInfo>(m_instance, measurement_pixel_min, measurement_neighbour_img);
}

} // end SExtractor

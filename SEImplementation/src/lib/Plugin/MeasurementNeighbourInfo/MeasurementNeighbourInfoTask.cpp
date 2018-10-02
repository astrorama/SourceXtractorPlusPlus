/*
 * MeasurementNeighbourInfoTask.cpp
 *
 *  Created on: Sep 28, 2018
 *      Author: Alejandro Alvarez Ayllon
 */

#include <SEImplementation/Plugin/MeasurementFrameRectangle/MeasurementFrameRectangle.h>
#include "SEFramework/Property/DetectionFrame.h"
#include "SEImplementation/Plugin/PixelBoundaries/PixelBoundaries.h"
#include "SEImplementation/Property/PixelCoordinateList.h"
#include "SEImplementation/Plugin/MeasurementFrame/MeasurementFrame.h"
#include "SEImplementation/Plugin/MeasurementNeighbourInfo/MeasurementNeighbourInfoTask.h"
#include "SEImplementation/Plugin/MeasurementNeighbourInfo/MeasurementNeighbourInfo.h"

namespace SExtractor {

MeasurementNeighbourInfoTask::MeasurementNeighbourInfoTask(unsigned instance) : m_instance{instance} {}

void MeasurementNeighbourInfoTask::computeProperties(SExtractor::SourceInterface &source) const {
  const auto &measurement_frame = source.getProperty<MeasurementFrame>(m_instance).getFrame();
  const auto &measurement_boundaries = source.getProperty<MeasurementFrameRectangle>(m_instance);
  const auto &detection_frame = source.getProperty<DetectionFrame>().getFrame();
  const auto &detection_pixel_list = source.getProperty<PixelCoordinateList>().getCoordinateList();
  const auto &detection_boundaries = source.getProperty<PixelBoundaries>();

  const auto &measurement_coordinates = measurement_frame->getCoordinateSystem();

  const auto &detection_threshold = detection_frame->getThresholdedImage();
  const auto &detection_coordinates = detection_frame->getCoordinateSystem();

  const auto &detection_pixel_min = detection_boundaries.getMin();

  // If the source is outside bounds, skip
  if (measurement_boundaries.getWidth() <= 0) {
    source.setIndexedProperty<MeasurementNeighbourInfo>(m_instance);
    return;
  }

  // Create first an image, in the detection image reference frame, where pixels are -1 if they belong
  // to the source, 0 otherwise
  auto detection_neighbour_img = VectorImage<int>::create(detection_boundaries.getWidth(),
                                                          detection_boundaries.getHeight());

  for (auto pixel_coord : detection_pixel_list) {
    auto act_x = pixel_coord.m_x - detection_pixel_min.m_x;
    auto act_y = pixel_coord.m_y - detection_pixel_min.m_y;

    // make sure to be inside the image
    assert(act_x >= 0 && act_y >= 0 && act_x < detection_neighbour_img->getWidth() &&
           act_y < detection_neighbour_img->getHeight());
    detection_neighbour_img->setValue(act_x, act_y, -1);
  }

  // Set surrounding pixels above the threshold to 1, all others to 0
  for (int act_y = 0; act_y < detection_neighbour_img->getHeight(); act_y++) {
    for (int act_x = 0; act_x < detection_neighbour_img->getWidth(); act_x++) {
      int dx = act_x + detection_pixel_min.m_x;
      int dy = act_y + detection_pixel_min.m_y;

      if (detection_threshold->getValue(dx, dy) > 0 && detection_neighbour_img->getValue(act_x, act_y) != -1) {
        detection_neighbour_img->setValue(act_x, act_y, 1);
      } else {
        detection_neighbour_img->setValue(act_x, act_y, 0);
      }
    }
  }

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

      if (dy < 0 || dx < 0 || dx >= detection_neighbour_img->getWidth() || dy >= detection_neighbour_img->getHeight()) {
        measurement_neighbour_img->setValue(mx, my, 0);
      } else {
        measurement_neighbour_img->setValue(mx, my, detection_neighbour_img->getValue(dx, dy));
      }
    }
  }

  // Store as a property
  source.setIndexedProperty<MeasurementNeighbourInfo>(m_instance, measurement_pixel_min, measurement_neighbour_img);
}

} // end SExtractor

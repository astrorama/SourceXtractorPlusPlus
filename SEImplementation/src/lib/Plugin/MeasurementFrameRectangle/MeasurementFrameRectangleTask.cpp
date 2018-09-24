/*
 * MeasurementFrameRectangleTask.cpp
 *
 *  Created on: Sep 24, 2018
 *      Author: Alejandro Alvarez Ayllon
 */

#include <iostream>
#include <mutex>
#include <SEFramework/Property/DetectionFrame.h>
#include <SEImplementation/Plugin/MeasurementFrame/MeasurementFrame.h>
#include <SEImplementation/Plugin/MeasurementFrameRectangle/MeasurementFrameRectangle.h>
#include <SEImplementation/Plugin/MeasurementFrameRectangle/MeasurementFrameRectangleTask.h>
#include <SEImplementation/Plugin/PixelBoundaries/PixelBoundaries.h>


namespace SExtractor {

void MeasurementFrameRectangleTask::computeProperties(SourceInterface& source) const {
  auto frame = source.getProperty<MeasurementFrame>(m_instance).getFrame();
  auto frame_coordinates = frame->getCoordinateSystem();
  auto& pixel_boundaries = source.getProperty<PixelBoundaries>();
  auto detection_frame_coordinates = source.getProperty<DetectionFrame>().getFrame()->getCoordinateSystem();

  // Get the coordinates of the detection frame source stamp
  auto stamp_top_left = pixel_boundaries.getMin();
  auto width = pixel_boundaries.getWidth();
  auto height = pixel_boundaries.getHeight();

  // Transform the 4 corner coordinates from detection image
  auto coord1 = frame_coordinates->worldToImage(detection_frame_coordinates->imageToWorld(ImageCoordinate(
    stamp_top_left.m_x, stamp_top_left.m_y)));
  auto coord2 = frame_coordinates->worldToImage(detection_frame_coordinates->imageToWorld(ImageCoordinate(
    stamp_top_left.m_x + width, stamp_top_left.m_y)));
  auto coord3 = frame_coordinates->worldToImage(detection_frame_coordinates->imageToWorld(ImageCoordinate(
    stamp_top_left.m_x + width, stamp_top_left.m_y + height)));
  auto coord4 = frame_coordinates->worldToImage(detection_frame_coordinates->imageToWorld(ImageCoordinate(
    stamp_top_left.m_x, stamp_top_left.m_y + height)));

  // Determine the min/max coordinates
  auto min_x = std::min(coord1.m_x, std::min(coord2.m_x, std::min(coord3.m_x, coord4.m_x)));
  auto min_y = std::min(coord1.m_y, std::min(coord2.m_y, std::min(coord3.m_y, coord4.m_y)));
  auto max_x = std::max(coord1.m_x, std::max(coord2.m_x, std::max(coord3.m_x, coord4.m_x)));
  auto max_y = std::max(coord1.m_y, std::max(coord2.m_y, std::max(coord3.m_y, coord4.m_y)));

  PixelCoordinate min_coord, max_coord;
  min_coord.m_x = int(min_x);
  min_coord.m_y = int(min_y);
  max_coord.m_x = int(max_x) + 1;
  max_coord.m_y = int(max_y) + 1;

  // Clip the coordinates to fit the available image
  auto frame_image = frame->getSubtractedImage();
  min_coord.m_x = std::max(0, min_coord.m_x);
  min_coord.m_y = std::max(0, min_coord.m_y);
  max_coord.m_x = std::min(frame_image->getWidth(), max_coord.m_x);
  max_coord.m_y = std::min(frame_image->getHeight(), max_coord.m_y);

  source.setIndexedProperty<MeasurementFrameRectangle>(m_instance, min_coord, max_coord);
}

} // SEImplementation namespace


/*
 * MeasurementFramePixelCentroidTask.cpp
 *
 *  Created on: Oct 31, 2016
 *      Author: mschefer
 */

#include "SEFramework/Property/DetectionFrame.h"
#include "SEImplementation/Plugin/MeasurementFrame/MeasurementFrame.h"

#include "SEImplementation/Plugin/PixelCentroid/PixelCentroid.h"
#include "SEImplementation/Plugin/MeasurementFramePixelCentroid/MeasurementFramePixelCentroid.h"
#include "SEImplementation/Plugin/MeasurementFramePixelCentroid/MeasurementFramePixelCentroidTask.h"

namespace SExtractor {

void MeasurementFramePixelCentroidTask::computeProperties(SourceInterface& source) const {
  auto detection_coordinate_system = source.getProperty<DetectionFrame>().getCoordinateSystem();
  auto measurement_coordinate_system = source.getProperty<MeasurementFrame>(m_instance).getCoordinateSystem();
  auto pixel_centroid = source.getProperty<PixelCentroid>();

  ImageCoordinate detection_image_coordinate(pixel_centroid.getCentroidX(), pixel_centroid.getCentroidY());
  auto world_coordinate = detection_coordinate_system->imageToWorld(detection_image_coordinate);
  auto measurement_image_coordinate = measurement_coordinate_system->worldToImage(world_coordinate);

  source.setIndexedProperty<MeasurementFramePixelCentroid>(
      m_instance, measurement_image_coordinate.m_x, measurement_image_coordinate.m_y);
}

}


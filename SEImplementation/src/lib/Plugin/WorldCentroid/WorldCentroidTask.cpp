/*
 * WorldCentroidTask.cpp
 *
 *  Created on: Nov 21, 2016
 *      Author: mschefer
 */

#include "SEFramework/SEFramework/Property/DetectionFrame.h"

#include "SEImplementation/Property/PixelCoordinateList.h"
#include "SEImplementation/Plugin/DetectionFramePixelValues/DetectionFramePixelValues.h"
#include "SEImplementation/Plugin/PixelCentroid/PixelCentroid.h"
#include "SEImplementation/Plugin/WorldCentroid/WorldCentroid.h"

#include "SEImplementation/Plugin/WorldCentroid/WorldCentroidTask.h"

namespace SExtractor {

void WorldCentroidTask::computeProperties(SourceInterface& source) const {
  auto detection_frame = source.getProperty<DetectionFrame>().getDetectionImage();
  auto coordinate_system = source.getProperty<DetectionFrame>().getCoordinateSystem();

  ImageCoordinate image_coordinate(
      source.getProperty<PixelCentroid>().getCentroidX(), source.getProperty<PixelCentroid>().getCentroidY());

  auto world_coordinate = coordinate_system->imageToWorld(image_coordinate);

  source.setProperty<WorldCentroid>(world_coordinate.m_alpha, world_coordinate.m_delta);
}

}





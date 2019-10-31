/** Copyright © 2019 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3.0 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */
/*
 * WorldCentroidTask.cpp
 *
 *  Created on: Nov 21, 2016
 *      Author: mschefer
 */

#include "SEFramework/Property/DetectionFrame.h"

#include "SEImplementation/Property/PixelCoordinateList.h"
#include "SEImplementation/Plugin/DetectionFramePixelValues/DetectionFramePixelValues.h"
#include "SEImplementation/Plugin/PixelCentroid/PixelCentroid.h"
#include "SEImplementation/Plugin/WorldCentroid/WorldCentroid.h"

#include "SEImplementation/Plugin/WorldCentroid/WorldCentroidTask.h"

namespace SourceXtractor {

void WorldCentroidTask::computeProperties(SourceInterface& source) const {
  auto coordinate_system = source.getProperty<DetectionFrame>().getFrame()->getCoordinateSystem();

  ImageCoordinate image_coordinate(
      source.getProperty<PixelCentroid>().getCentroidX(), source.getProperty<PixelCentroid>().getCentroidY());

  auto world_coordinate = coordinate_system->imageToWorld(image_coordinate);

  source.setProperty<WorldCentroid>(world_coordinate.m_alpha, world_coordinate.m_delta);
}

}


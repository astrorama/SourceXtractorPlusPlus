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
 * MeasurementFramePixelCentroidTask.cpp
 *
 *  Created on: Oct 31, 2016
 *      Author: mschefer
 */

#include "SEImplementation/Plugin/MeasurementFrameCoordinates/MeasurementFrameCoordinates.h"
#include "SEImplementation/Plugin/DetectionFrameCoordinates/DetectionFrameCoordinates.h"

#include "SEImplementation/Plugin/PixelCentroid/PixelCentroid.h"
#include "SEImplementation/Plugin/MeasurementFramePixelCentroid/MeasurementFramePixelCentroid.h"
#include "SEImplementation/Plugin/MeasurementFramePixelCentroid/MeasurementFramePixelCentroidTask.h"

namespace SourceXtractor {

void MeasurementFramePixelCentroidTask::computeProperties(SourceInterface& source) const {
  auto detection_coordinate_system = source.getProperty<DetectionFrameCoordinates>().getCoordinateSystem();
  auto measurement_coordinate_system = source.getProperty<MeasurementFrameCoordinates>(m_instance).getCoordinateSystem();
  auto pixel_centroid = source.getProperty<PixelCentroid>();

  ImageCoordinate detection_image_coordinate(pixel_centroid.getCentroidX(), pixel_centroid.getCentroidY());
  auto world_coordinate = detection_coordinate_system->imageToWorld(detection_image_coordinate);

  try {
    auto measurement_image_coordinate = measurement_coordinate_system->worldToImage(world_coordinate);
    source.setIndexedProperty<MeasurementFramePixelCentroid>(m_instance, measurement_image_coordinate.m_x,
                                                             measurement_image_coordinate.m_y);
  }
  catch (const InvalidCoordinatesException&) {
    source.setIndexedProperty<MeasurementFramePixelCentroid>(m_instance, true);
  }
}

}


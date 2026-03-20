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

#include <iostream>

#include <SEImplementation/Plugin/MeasurementFrameCoordinates/MeasurementFrameCoordinates.h>
#include <SEImplementation/Plugin/MeasurementFrameInfo/MeasurementFrameInfo.h>
#include <SEImplementation/Plugin/PixelBoundaries/PixelBoundaries.h>
#include "SEImplementation/Plugin/DetectionFrameCoordinates/DetectionFrameCoordinates.h"
#include <SEImplementation/Plugin/WorldCentroid/WorldCentroid.h>
#include <SEImplementation/Plugin/AssocMode/AssocMode.h>
#include <SEImplementation/Plugin/ReferenceCoordinates/ReferenceCoordinates.h>

#include <SEImplementation/Plugin/MeasurementFrameRectangle/MeasurementFrameRectangle.h>
#include <SEImplementation/Plugin/MeasurementFrameRectangle/MeasurementFrameRectangleTaskNoDetect.h>

namespace SourceXtractor {

void MeasurementFrameRectangleTaskNoDetect::computeProperties(SourceInterface& source) const {
  auto measurement_frame_coordinates = source.getProperty<MeasurementFrameCoordinates>(m_instance).getCoordinateSystem();
  auto reference_frame_coordinates = source.getProperty<ReferenceCoordinates>().getCoordinateSystem();

  const auto& measurement_frame_info = source.getProperty<MeasurementFrameInfo>(m_instance);
  const auto& world_centroid = source.getProperty<WorldCentroid>();
  const auto& assoc_mode = source.getProperty<AssocMode>();

  bool bad_coordinates = false;
  ImageCoordinate coord1, coord2, coord3, coord4;
  try {
    auto w = assoc_mode.getRefFramePixelWidth();
    auto h = assoc_mode.getRefFramePixelHeight();

    auto c = reference_frame_coordinates->worldToImage(world_centroid.getCentroid());
    coord1 = measurement_frame_coordinates->worldToImage(
        reference_frame_coordinates->imageToWorld(ImageCoordinate(c.m_x - w, c.m_y - h)));
    coord2 = measurement_frame_coordinates->worldToImage(
        reference_frame_coordinates->imageToWorld(ImageCoordinate(c.m_x + w, c.m_y - h)));
    coord3 = measurement_frame_coordinates->worldToImage(
        reference_frame_coordinates->imageToWorld(ImageCoordinate(c.m_x - w, c.m_y + h)));
    coord4 = measurement_frame_coordinates->worldToImage(
        reference_frame_coordinates->imageToWorld(ImageCoordinate(c.m_x + w, c.m_y + h)));
  }
  catch (const InvalidCoordinatesException&) {
    bad_coordinates = true;
  }

  // Determine the min/max coordinates
  auto min_x = std::min(coord1.m_x, std::min(coord2.m_x, std::min(coord3.m_x, coord4.m_x)));
  auto min_y = std::min(coord1.m_y, std::min(coord2.m_y, std::min(coord3.m_y, coord4.m_y)));
  auto max_x = std::max(coord1.m_x, std::max(coord2.m_x, std::max(coord3.m_x, coord4.m_x)));
  auto max_y = std::max(coord1.m_y, std::max(coord2.m_y, std::max(coord3.m_y, coord4.m_y)));

  // The full boundaries may lie outside of the frame
  if (bad_coordinates || max_x < 0.0 || max_y < 0.0 ||
      int(min_x) >= measurement_frame_info.getWidth() || int(max_y) >= measurement_frame_info.getHeight()) {
    source.setIndexedProperty<MeasurementFrameRectangle>(m_instance, bad_coordinates);
  }
  // Clip the coordinates to fit the available image
  else {
    min_x = std::max(0.0, min_x);
    min_y = std::max(0.0, min_y);
    max_x = std::min(static_cast<double>(measurement_frame_info.getWidth()), max_x);
    max_y = std::min(static_cast<double>(measurement_frame_info.getHeight()), max_y);

    source.setIndexedProperty<MeasurementFrameRectangle>(
      m_instance, ImageCoordinate(min_x, min_y), ImageCoordinate(max_x, max_y));
  }
}

} // SEImplementation namespace


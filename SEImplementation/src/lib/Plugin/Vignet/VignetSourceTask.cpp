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

/**
 * @file VignetSourceTask.cpp
 *
 * @date Jan. 23, 2020
 * @author mkuemmel@usm.lmu.de
 */

#include "SEFramework/Image/RecenterImage.h"
#include "SEFramework/Image/PaddedImage.h"
#include "SEImplementation/Property/PixelCoordinateList.h"
#include "SEImplementation/Plugin/MeasurementFrameInfo/MeasurementFrameInfo.h"
#include "SEImplementation/Plugin/MeasurementFrameCoordinates/MeasurementFrameCoordinates.h"
#include "SEImplementation/Plugin/MeasurementFrameImages/MeasurementFrameImages.h"
#include "SEImplementation/Plugin/MeasurementFramePixelCentroid/MeasurementFramePixelCentroid.h"
#include "SEImplementation/Plugin/DetectionFrameCoordinates/DetectionFrameCoordinates.h"
#include "SEImplementation/Plugin/DetectionFrameImages/DetectionFrameImages.h"
#include "SEImplementation/Plugin/Vignet/Vignet.h"
#include "SEImplementation/Plugin/Vignet/VignetSourceTask.h"

namespace SourceXtractor {
void VignetSourceTask::computeProperties(SourceInterface& source) const {
  const auto& measurement_frame_info = source.getProperty<MeasurementFrameInfo>(m_instance);
  const auto& measurement_frame_images = source.getProperty<MeasurementFrameImages>(m_instance);

  auto measurement_var_threshold = measurement_frame_info.getVarianceThreshold();

  const auto measurement_sub_image = measurement_frame_images.getImage(LayerSubtractedImage);


  const auto& detection_frame_images = source.getProperty<DetectionFrameImages>();

    // get the object pixel coordinates from the detection image
  const auto& pixel_coords = source.getProperty<PixelCoordinateList>();

  // coordinate systems
  auto detection_coordinate_system = source.getProperty<DetectionFrameCoordinates>().getCoordinateSystem();
  auto measurement_coordinate_system = source.getProperty<MeasurementFrameCoordinates>(
    m_instance).getCoordinateSystem();

  // get the central pixel coord
  const auto& centroid = source.getProperty<MeasurementFramePixelCentroid>(m_instance);
  const int x_pix = static_cast<int>(centroid.getCentroidX() + 0.5);
  const int y_pix = static_cast<int>(centroid.getCentroidY() + 0.5);

  // get the sub-image boundaries
  int x_start = std::max(x_pix - m_vignet_size[0] / 2, 0);
  int y_start = std::max(y_pix - m_vignet_size[1] / 2, 0);
  int x_end = std::min(x_start + m_vignet_size[0], measurement_sub_image->getWidth() - 1);
  int y_end = std::min(y_start + m_vignet_size[1], measurement_sub_image->getHeight() - 1);

  int w = x_end - x_start + 1;
  int h = y_end - y_start + 1;

  // neighbor masking from the detection image
  const auto& detection_thresh_image = detection_frame_images.getImageChunk(LayerThresholdedImage, x_start, y_start, w, h);

  // measurement variance
  const auto measurement_var_image = measurement_frame_images.getImageChunk(LayerVarianceMap, x_start, y_start, w, h);

  // cutout, clipped to the image
  auto vignet = VectorImage<DetectionImage::PixelType>::create(
    std::move(*measurement_sub_image->getChunk(x_start, y_start, w, h)));

  // mask pixels
  for (int iy = y_start; iy <= y_end; ++iy) {
    for (int ix = x_start; ix <= x_end; ++ix) {
      // translate pixel coordinates to the detection frame
      auto world_coord = measurement_coordinate_system->imageToWorld(
        {static_cast<double>(ix), static_cast<double>(iy)});
      auto detection_coord = detection_coordinate_system->worldToImage(world_coord);

      // if it corresponds to a detection pixel, use it if it belongs to the source
      int detection_x = static_cast<int>(detection_coord.m_x + 0.5);
      int detection_y = static_cast<int>(detection_coord.m_y + 0.5);

      bool is_masked = measurement_var_image->getValue(ix, iy) > measurement_var_threshold;
      bool is_detection_pixel = detection_thresh_image->getValue(detection_x, detection_y) > 0;
      bool in_coord_list = pixel_coords.contains({detection_x, detection_y});

      if (is_masked || !is_detection_pixel || !in_coord_list) {
        vignet->setValue(ix - x_start, iy - y_start, m_vignet_default_pixval);
      }
    }
  }

  // pad
  if (w < m_vignet_size[0] || h < m_vignet_size[1]) {
    auto padded = PaddedImage<DetectionImage::PixelType>::create(std::move(vignet),
                                                                 m_vignet_size[0],
                                                                 m_vignet_size[1],
                                                                 m_vignet_default_pixval);
    vignet = VectorImage<DetectionImage::PixelType>::create(padded);
  }

  // set property
  source.setIndexedProperty<Vignet>(m_instance, std::move(vignet));
}
} // namespace SourceXtractor

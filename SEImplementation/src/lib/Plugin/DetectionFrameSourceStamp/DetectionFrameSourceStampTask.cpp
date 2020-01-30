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
 * @file src/lib/Task/DetectionFrameSourceStampTask.cpp
 * @date 06/16/16
 * @author mschefer
 */

#include <mutex>

#include "SEFramework/Image/Image.h"
#include "SEFramework/Image/VectorImage.h"
#include "SEFramework/Property/DetectionFrame.h"
#include "SEImplementation/Plugin/PixelBoundaries/PixelBoundaries.h"

#include "SEImplementation/Plugin/DetectionFrameSourceStamp/DetectionFrameSourceStamp.h"
#include "SEImplementation/Plugin/DetectionFrameSourceStamp/DetectionFrameSourceStampTask.h"

#include "SEImplementation/Measurement/MultithreadedMeasurement.h"

namespace SourceXtractor {

void DetectionFrameSourceStampTask::computeProperties(SourceInterface& source) const {
  std::lock_guard<std::recursive_mutex> lock(MultithreadedMeasurement::g_global_mutex);

  auto detection_frame = source.getProperty<DetectionFrame>().getFrame();
  auto subtracted_image = detection_frame->getSubtractedImage();
  auto thresholded_image = detection_frame->getThresholdedImage();
  auto variance_map = detection_frame->getVarianceMap();
  auto threshold_map = detection_frame->getDetectionThresholdMap();

  const auto& boundaries = source.getProperty<PixelBoundaries>();
  auto min = boundaries.getMin();
  auto max = boundaries.getMax();


  // FIXME temporary, for now just enlarge the area by a fixed amount of pixels
  PixelCoordinate border = (max - min) * .8 + PixelCoordinate(2, 2);

  min -= border;
  max += border;

  // clip to image size
  min.m_x = std::max(min.m_x, 0);
  min.m_y = std::max(min.m_y, 0);
  max.m_x = std::min(max.m_x, subtracted_image->getWidth() - 1);
  max.m_y = std::min(max.m_y, subtracted_image->getHeight() - 1);

  auto width = max.m_x - min.m_x +1;
  auto height = max.m_y - min.m_y + 1;

  std::vector<DetectionImage::PixelType> data (width * height);
  std::vector<DetectionImage::PixelType> thresholded_data (width * height);
  std::vector<DetectionImage::PixelType> variance_data (width * height);
  std::vector<DetectionImage::PixelType> threshold_map_data (width * height);

  for (auto x = min.m_x; x <= max.m_x; ++x) {
    for (auto y = min.m_y; y <= max.m_y; ++y) {
      auto index = (x-min.m_x) + (y-min.m_y) * width;
      data[index] = subtracted_image->getValue(x, y);
      thresholded_data[index] = thresholded_image->getValue(x, y);
      variance_data[index] = variance_map->getValue(x, y);
      threshold_map_data[index] = threshold_map->getValue(x, y);
    }
  }

  std::shared_ptr<DetectionImage> stamp = VectorImage<DetectionImage::PixelType>::create(width, height, data);
  std::shared_ptr<DetectionImage> thresholded_stamp =
      VectorImage<DetectionImage::PixelType>::create(width, height, thresholded_data);
  std::shared_ptr<WeightImage> variance_stamp =
      VectorImage<WeightImage::PixelType>::create(width, height, variance_data);
  std::shared_ptr<DetectionImage> threshold_map_stamp =
      VectorImage<DetectionImage::PixelType>::create(width, height, threshold_map_data);

  source.setProperty<DetectionFrameSourceStamp>(stamp, thresholded_stamp, min, variance_stamp, threshold_map_stamp);
}

} // SEImplementation namespace




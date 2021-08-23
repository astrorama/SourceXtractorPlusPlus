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

#include <SEFramework/Property/DetectionFrame.h>
#include <mutex>

#include "SEFramework/Image/Image.h"
#include "SEFramework/Image/SubImage.h"

#include "SEImplementation/Measurement/MultithreadedMeasurement.h"
#include "SEImplementation/Plugin/DetectionFrameImages/DetectionFrameImages.h"
#include "SEImplementation/Plugin/DetectionFrameSourceStamp/DetectionFrameSourceStamp.h"
#include "SEImplementation/Plugin/DetectionFrameSourceStamp/DetectionFrameSourceStampTask.h"
#include "SEImplementation/Plugin/PixelBoundaries/PixelBoundaries.h"

namespace SourceXtractor {

void DetectionFrameSourceStampTask::computeProperties(SourceInterface& source) const {
  using SubDetection = SubImage<DetectionImage::PixelType>;
  using SubWeight    = SubImage<WeightImage::PixelType>;
  auto det_frame     = source.getProperty<DetectionFrameImages>();

  const auto& boundaries = source.getProperty<PixelBoundaries>();
  auto        min        = boundaries.getMin();
  auto        max        = boundaries.getMax();

  // FIXME temporary, for now just enlarge the area by a fixed amount of pixels
  PixelCoordinate border = (max - min) * .8 + PixelCoordinate(2, 2);

  min -= border;
  max += border;

  // clip to image size
  min.m_x = std::max(min.m_x, 0);
  min.m_y = std::max(min.m_y, 0);
  max.m_x = std::min(max.m_x, det_frame.getWidth() - 1);
  max.m_y = std::min(max.m_y, det_frame.getHeight() - 1);

  auto width  = max.m_x - min.m_x + 1;
  auto height = max.m_y - min.m_y + 1;

  auto stamp               = SubDetection::create(det_frame.getImage(LayerSubtractedImage), min, width, height);
  auto filtered_stamp      = SubDetection::create(det_frame.getImage(LayerFilteredImage), min, width, height);
  auto thresholded_stamp   = SubDetection::create(det_frame.getImage(LayerThresholdedImage), min, width, height);
  auto threshold_map_stamp = SubDetection::create(det_frame.getImage(LayerDetectionThresholdMap), min, width, height);
  auto variance_stamp      = SubWeight::create(det_frame.getImage(LayerVarianceMap), min, width, height);

  source.setProperty<DetectionFrameSourceStamp>(stamp, filtered_stamp, thresholded_stamp, min, variance_stamp, threshold_map_stamp);
}

}  // namespace SourceXtractor

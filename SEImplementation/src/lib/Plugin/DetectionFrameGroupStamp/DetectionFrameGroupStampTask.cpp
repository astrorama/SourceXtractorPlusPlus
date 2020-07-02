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
 * DetectionFrameGroupStampTask.cpp
 *
 *  Created on: May 5, 2017
 *      Author: mschefer
 */

#include "SEFramework/Image/Image.h"
#include "SEFramework/Image/VectorImage.h"

#include "SEImplementation/Plugin/PixelBoundaries/PixelBoundaries.h"
#include "SEImplementation/Plugin/DetectionFrameImages/DetectionFrameImages.h"

#include "SEImplementation/Plugin/DetectionFrameGroupStamp/DetectionFrameGroupStamp.h"
#include "SEImplementation/Plugin/DetectionFrameGroupStamp/DetectionFrameGroupStampTask.h"


namespace SourceXtractor {

void DetectionFrameGroupStampTask::computeProperties(SourceGroupInterface& group) const {
  // we are obviously assuming the same DetectionFrameImages for all sources
  auto detection_frame_images = group.cbegin()->getProperty<DetectionFrameImages>();

  //////////////// FIXME move to its own property?
  int min_x = INT_MAX;
  int min_y = INT_MAX;
  int max_x = INT_MIN;
  int max_y = INT_MIN;

  for (auto& source : group) {
    const auto& boundaries = source.getProperty<PixelBoundaries>();
    const auto& min = boundaries.getMin();
    const auto& max = boundaries.getMax();

    min_x = std::min(min_x, min.m_x);
    min_y = std::min(min_y, min.m_y);
    max_x = std::max(max_x, max.m_x);
    max_y = std::max(max_y, max.m_y);
  }
  PixelCoordinate max(max_x, max_y);
  PixelCoordinate min(min_x, min_y);
  ///////////////////////////////////////


  // FIXME temporary, for now just enlarge the area by a fixed amount of pixels
  PixelCoordinate border = (max - min) * .8 + PixelCoordinate(2, 2);

  min -= border;
  max += border;

  // clip to image size
  min.m_x = std::max(min.m_x, 0);
  min.m_y = std::max(min.m_y, 0);
  max.m_x = std::min(max.m_x, detection_frame_images.getWidth() - 1);
  max.m_y = std::min(max.m_y, detection_frame_images.getHeight() - 1);

  // create the image stamp
  auto width = max.m_x - min.m_x +1;
  auto height = max.m_y - min.m_y + 1;

  std::shared_ptr<DetectionImage> stamp = VectorImage<DetectionImage::PixelType>::create(
      *detection_frame_images.getImageChunk(LayerSubtractedImage, min.m_x, min.m_y, width, height));
  std::shared_ptr<DetectionImage> thresholded_stamp = VectorImage<DetectionImage::PixelType>::create(
      *detection_frame_images.getImageChunk(LayerThresholdedImage, min.m_x, min.m_y, width, height));
  std::shared_ptr<WeightImage> variance_stamp = VectorImage<WeightImage::PixelType>::create(
      *detection_frame_images.getImageChunk(LayerVarianceMap, min.m_x, min.m_y, width, height));


  group.setProperty<DetectionFrameGroupStamp>(stamp, thresholded_stamp, min, variance_stamp);
}

} // SEImplementation namespace


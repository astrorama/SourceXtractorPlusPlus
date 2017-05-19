/*
 * DetectionFrameGroupStampTask.cpp
 *
 *  Created on: May 5, 2017
 *      Author: mschefer
 */

#include <iostream>

#include "SEFramework/Image/Image.h"
#include "SEFramework/Image/VectorImage.h"
#include "SEFramework/Property/DetectionFrame.h"
#include "SEImplementation/Plugin/PixelBoundaries/PixelBoundaries.h"

#include "SEImplementation/Plugin/DetectionFrameGroupStamp/DetectionFrameGroupStamp.h"
#include "SEImplementation/Plugin/DetectionFrameGroupStamp/DetectionFrameGroupStampTask.h"

namespace SExtractor {

void DetectionFrameGroupStampTask::computeProperties(SourceGroupInterface& group) const {

  // FIXME we are obviously assuming the same DetectionFrame for all sources but we should not have to rely on that
  auto detection_frame = group.cbegin()->getProperty<DetectionFrame>().getFrame()->getSubtractedImage();

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
  PixelCoordinate border = (max - min) * .5 + PixelCoordinate(2, 2);

  min -= border;
  max += border;

  // clip to image size
  min.m_x = std::max(min.m_x, 0);
  min.m_y = std::max(min.m_y, 0);
  max.m_x = std::min(max.m_x, detection_frame->getWidth()-1);
  max.m_y = std::min(max.m_y, detection_frame->getHeight()-1);

  // create the image stamp
  auto width = max.m_x - min.m_x +1;
  auto height = max.m_y - min.m_y + 1;
  std::vector<DetectionImage::PixelType> data (width * height);

  // copy the data
  for (auto x = min.m_x; x <= max.m_x; ++x) {
    for (auto y = min.m_y; y <= max.m_y; ++y) {
      data[(x-min.m_x) + (y-min.m_y) * width] = detection_frame->getValue(x, y);
    }
  }

  // set the property
  std::shared_ptr<DetectionImage> stamp {new VectorImage<DetectionImage::PixelType>(width, height, data)};
  group.setProperty<DetectionFrameGroupStamp>(stamp, min);
}

} // SEImplementation namespace


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

namespace SExtractor {

void DetectionFrameSourceStampTask::computeProperties(SourceInterface& source) const {
  std::lock_guard<std::recursive_mutex> lock(MultithreadedMeasurement::g_global_mutex);

  auto detection_frame = source.getProperty<DetectionFrame>().getFrame();
  auto subtracted_image = detection_frame->getSubtractedImage();
  auto thresholded_image = detection_frame->getThresholdedImage();
  auto variance_map = detection_frame->getVarianceMap();

  const auto& boundaries = source.getProperty<PixelBoundaries>();
  auto min = boundaries.getMin();
  auto max = boundaries.getMax();


  // FIXME temporary, for now just enlarge the area by a fixed amount of pixels
  PixelCoordinate border = (max - min) * .8 + PixelCoordinate(5, 5);

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

  for (auto x = min.m_x; x <= max.m_x; ++x) {
    for (auto y = min.m_y; y <= max.m_y; ++y) {
      auto index = (x-min.m_x) + (y-min.m_y) * width;
      data[index] = subtracted_image->getValue(x, y);
      thresholded_data[index] = thresholded_image->getValue(x, y);
      variance_data[index] = variance_map->getValue(x, y);
    }
  }

  std::shared_ptr<DetectionImage> stamp = VectorImage<DetectionImage::PixelType>::create(width, height, data);
  std::shared_ptr<DetectionImage> thresholded_stamp =
      VectorImage<DetectionImage::PixelType>::create(width, height, thresholded_data);
  std::shared_ptr<WeightImage> variance_stamp =
      VectorImage<WeightImage::PixelType>::create(width, height, variance_data);

  source.setProperty<DetectionFrameSourceStamp>(stamp, thresholded_stamp, min, variance_stamp);
}

} // SEImplementation namespace




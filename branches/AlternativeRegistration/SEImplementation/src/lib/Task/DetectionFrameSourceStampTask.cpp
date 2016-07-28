/**
 * @file src/lib/Task/DetectionFrameSourceStampTask.cpp
 * @date 06/16/16
 * @author mschefer
 */
#include "SEFramework/Image/Image.h"
#include "SEFramework/Image/VectorImage.h"
#include "SEImplementation/Property/PixelBoundaries.h"

#include "SEImplementation/Property/DetectionFrameSourceStamp.h"
#include "SEImplementation/Task/DetectionFrameSourceStampTask.h"

namespace SExtractor {

void DetectionFrameSourceStampTask::computeProperties(PixelSourceInterface& source) const {
  const auto& boundaries = source.getProperty<PixelBoundaries>();
  const auto& min = boundaries.getMin();
  const auto& max = boundaries.getMax();

  auto width = max.m_x - min.m_x +1;
  auto height = max.m_y - min.m_y + 1;
  std::vector<double> data (width * height);
  for (auto x = min.m_x; x <= max.m_x; ++x) {
    for (auto y = min.m_y; y <= max.m_y; ++y) {
      data[(x-min.m_x) + (y-min.m_y) * width] = m_image->getValue(x, y);
    }
  }
  std::shared_ptr<DetectionImage> stamp {new VectorImage<DetectionImage::PixelType>(width, height, data)};
  source.setProperty(
      std::unique_ptr<DetectionFrameSourceStamp>(new DetectionFrameSourceStamp(stamp)));
}

} // SEImplementation namespace




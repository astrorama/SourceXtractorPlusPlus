/**
 * @file src/lib/Task/DetectionFramePixelValuesTask.cpp
 * @date 06/16/16
 * @author mschefer
 */
#include <memory>

#include "SEImplementation/Property/PixelCoordinateList.h"
#include "SEFramework/Property/DetectionFrame.h"

#include "SEImplementation/Plugin/DetectionFramePixelValues/DetectionFramePixelValues.h"
#include "SEImplementation/Plugin/DetectionFramePixelValues/DetectionFramePixelValuesTask.h"

namespace SExtractor {

void DetectionFramePixelValuesTask::computeProperties(SourceInterface& source) const {
  auto detection_frame = source.getProperty<DetectionFrame>().getDetectionImage();
  auto filtered_frame = source.getProperty<DetectionFrame>().getLabellingImage();

  std::vector<DetectionImage::PixelType> values, filtered_values;
  for (auto pixel_coord : source.getProperty<PixelCoordinateList>().getCoordinateList()) {
    values.push_back(detection_frame->getValue(pixel_coord.m_x, pixel_coord.m_y));
    filtered_values.push_back(filtered_frame->getValue(pixel_coord.m_x, pixel_coord.m_y));
  }

  source.setProperty<DetectionFramePixelValues>(std::move(values), std::move(filtered_values));
}

} // SEImplementation namespace


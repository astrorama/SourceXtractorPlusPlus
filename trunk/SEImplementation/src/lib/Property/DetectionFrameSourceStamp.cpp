/**
 * @file src/lib/DetectionFrameSourceStamp.cpp
 * @date 06/03/16
 * @author mschefer
 */

#include "SEFramework/Image/Image.h"

#include "SEImplementation/Property/PixelBoundaries.h"
#include "SEImplementation/Property//DetectionFrameSourceStamp.h"

#include "Configuration/ConfigManager.h"
#include "SEConfig/DetectionImageConfig.h"

using namespace Euclid::Configuration;
using namespace SEConfig;

namespace SEImplementation {


void DetectionFrameSourceStampTask::computeProperties(Source& source) const {
  const auto& boundaries = source.getProperty<PixelBoundaries>();

  auto stamp = m_image->getSubImage(boundaries.getMin(), boundaries.getMax());
  source.setProperty(
      std::unique_ptr<DetectionFrameSourceStamp>(new DetectionFrameSourceStamp(stamp)));
}

DetectionFrameSourceStampTaskFactory::DetectionFrameSourceStampTaskFactory()
  : m_detection_frame_source_stamp_task(
      std::make_shared<DetectionFrameSourceStampTask>(
          ConfigManager::getInstance(0).getConfiguration<DetectionImageConfig>().getDetectionImage())) {
}

std::shared_ptr<Task> DetectionFrameSourceStampTaskFactory::getTask(PropertyId property_id) {
  if (property_id == PropertyId(typeid(DetectionFrameSourceStamp))) {
    return m_detection_frame_source_stamp_task;
  } else {
    return nullptr;
  }
}

const std::vector<PropertyId> DetectionFrameSourceStampTaskFactory::getProducedProperties() {
  return { PropertyId(typeid(DetectionFrameSourceStamp)) };
}


} // SEImplementation namespace




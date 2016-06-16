/**
 * @file src/lib/Task/DetectionFrameSourceStampTaskFactory.cpp
 * @date 06/16/16
 * @author mschefer
 */

#include "Configuration/ConfigManager.h"
#include "SEConfig/DetectionImageConfig.h"

using namespace Euclid::Configuration;


#include "SEImplementation/Property/DetectionFrameSourceStamp.h"
#include "SEImplementation/Task/DetectionFrameSourceStampTask.h"
#include "SEImplementation/Task/DetectionFrameSourceStampTaskFactory.h"

namespace SExtractor {

DetectionFrameSourceStampTaskFactory::DetectionFrameSourceStampTaskFactory()
  : m_detection_frame_source_stamp_task(
      std::make_shared<DetectionFrameSourceStampTask>(
          ConfigManager::getInstance(0).getConfiguration<DetectionImageConfig>().getDetectionImage())) {
}

std::shared_ptr<Task> DetectionFrameSourceStampTaskFactory::getTask(const PropertyId& property_id) {
  if (property_id == PropertyId::create<DetectionFrameSourceStamp>()) {
    return m_detection_frame_source_stamp_task;
  } else {
    return nullptr;
  }
}

const std::vector<PropertyId> DetectionFrameSourceStampTaskFactory::getProducedProperties() {
  return { PropertyId::create<DetectionFrameSourceStamp>() };
}

} // SEImplementation namespace




/**
 * @file src/lib/Task/DetectionFrameSourceStampTaskFactory.cpp
 * @date 06/16/16
 * @author mschefer
 */

#include "Configuration/ConfigManager.h"
#include "SEImplementation/Configuration/DetectionImageConfig.h"

using namespace Euclid::Configuration;


#include "SEImplementation/Property/DetectionFrameSourceStamp.h"
#include "SEImplementation/Task/DetectionFrameSourceStampTask.h"
#include "SEImplementation/Task/DetectionFrameSourceStampTaskFactory.h"

namespace SExtractor {

void DetectionFrameSourceStampTaskFactory::reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) {
  manager.registerConfiguration<DetectionImageConfig>();
}

void DetectionFrameSourceStampTaskFactory::configure(Euclid::Configuration::ConfigManager& manager) {
  m_detection_frame_source_stamp_task = std::make_shared<DetectionFrameSourceStampTask>(
          manager.getConfiguration<DetectionImageConfig>().getDetectionImage());
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




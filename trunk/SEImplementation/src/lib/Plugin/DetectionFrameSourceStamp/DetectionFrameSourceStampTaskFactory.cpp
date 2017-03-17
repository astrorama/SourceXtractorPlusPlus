/**
 * @file src/lib/Task/DetectionFrameSourceStampTaskFactory.cpp
 * @date 06/16/16
 * @author mschefer
 */

#include "Configuration/ConfigManager.h"

#include "SEImplementation/Plugin/DetectionFrameSourceStamp/DetectionFrameSourceStamp.h"
#include "SEImplementation/Plugin/DetectionFrameSourceStamp/DetectionFrameSourceStampTask.h"
#include "SEImplementation/Plugin/DetectionFrameSourceStamp/DetectionFrameSourceStampTaskFactory.h"

using namespace Euclid::Configuration;

namespace SExtractor {

std::shared_ptr<Task> DetectionFrameSourceStampTaskFactory::createTask(const PropertyId& property_id) const {
  if (property_id == PropertyId::create<DetectionFrameSourceStamp>()) {
    return std::make_shared<DetectionFrameSourceStampTask>();
  } else {
    return nullptr;
  }
}

} // SEImplementation namespace




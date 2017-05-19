/**
 * @file src/lib/Task/DetectionFramePixelValuesTaskFactory.cpp
 * @date 06/16/16
 * @author mschefer
 */

#include "Configuration/ConfigManager.h"

#include "SEImplementation/Plugin/DetectionFramePixelValues/DetectionFramePixelValues.h"
#include "SEImplementation/Plugin/DetectionFramePixelValues/DetectionFramePixelValuesTask.h"
#include "SEImplementation/Plugin/DetectionFramePixelValues/DetectionFramePixelValuesTaskFactory.h"

using namespace Euclid::Configuration;

namespace SExtractor {

std::shared_ptr<Task> DetectionFramePixelValuesTaskFactory::createTask(const PropertyId& property_id) const {
  if (property_id == PropertyId::create<DetectionFramePixelValues>()) {
    return std::make_shared<DetectionFramePixelValuesTask>();
  } else {
    return nullptr;
  }
}

} // SEImplementation namespace




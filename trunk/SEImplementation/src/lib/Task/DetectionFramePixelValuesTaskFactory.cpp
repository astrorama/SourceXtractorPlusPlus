/**
 * @file src/lib/Task/DetectionFramePixelValuesTaskFactory.cpp
 * @date 06/16/16
 * @author mschefer
 */

#include "Configuration/ConfigManager.h"
#include "SEConfig/DetectionImageConfig.h"

#include "SEImplementation/Property/DetectionFramePixelValues.h"
#include "SEImplementation/Task/DetectionFramePixelValuesTask.h"
#include "SEImplementation/Task/DetectionFramePixelValuesTaskFactory.h"

using namespace Euclid::Configuration;

namespace SExtractor {

DetectionFramePixelValuesTaskFactory::DetectionFramePixelValuesTaskFactory()
  : m_detection_frame_pixel_values_task(std::make_shared<DetectionFramePixelValuesTask>(
      ConfigManager::getInstance(0).getConfiguration<DetectionImageConfig>().getDetectionImage())) {
}

std::shared_ptr<Task> DetectionFramePixelValuesTaskFactory::getTask(const PropertyId& property_id) {
  if (property_id == PropertyId::create<DetectionFramePixelValues>()) {
    return m_detection_frame_pixel_values_task;
  } else {
    return nullptr;
  }
}

const std::vector<PropertyId> DetectionFramePixelValuesTaskFactory::getProducedProperties() {
  return { PropertyId::create<DetectionFramePixelValues>() };
}

} // SEImplementation namespace




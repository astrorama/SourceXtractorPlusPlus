/**
 * @file src/lib/Task/DetectionFramePixelValuesTaskFactory.cpp
 * @date 06/16/16
 * @author mschefer
 */

#include "Configuration/ConfigManager.h"

#include "SEImplementation/Configuration/DetectionImageConfig.h"
#include "SEImplementation/Plugin/DetectionFramePixelValues/DetectionFramePixelValues.h"
#include "SEImplementation/Plugin/DetectionFramePixelValues/DetectionFramePixelValuesTask.h"
#include "SEImplementation/Plugin/DetectionFramePixelValues/DetectionFramePixelValuesTaskFactory.h"

using namespace Euclid::Configuration;

namespace SExtractor {

void DetectionFramePixelValuesTaskFactory::reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const {
  manager.registerConfiguration<DetectionImageConfig>();
}

void DetectionFramePixelValuesTaskFactory::configure(Euclid::Configuration::ConfigManager& manager) {
  m_detection_frame_pixel_values_task = std::make_shared<DetectionFramePixelValuesTask>(
      manager.getConfiguration<DetectionImageConfig>().getDetectionImage());
}

std::shared_ptr<Task> DetectionFramePixelValuesTaskFactory::getTask(const PropertyId& property_id) const {
  if (property_id == PropertyId::create<DetectionFramePixelValues>()) {
    return m_detection_frame_pixel_values_task;
  } else {
    return nullptr;
  }
}

} // SEImplementation namespace




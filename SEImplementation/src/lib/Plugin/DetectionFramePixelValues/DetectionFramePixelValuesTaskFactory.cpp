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
  m_detection_image = manager.getConfiguration<DetectionImageConfig>().getDetectionImage();
}

std::shared_ptr<Task> DetectionFramePixelValuesTaskFactory::createTask(const PropertyId& property_id) const {
  if (property_id == PropertyId::create<DetectionFramePixelValues>()) {
    return std::make_shared<DetectionFramePixelValuesTask>(m_detection_image);
  } else {
    return nullptr;
  }
}

} // SEImplementation namespace




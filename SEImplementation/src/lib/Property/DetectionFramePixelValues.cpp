/**
 * @file src/lib/DetectionFramePixelValues.cpp
 * @date 05/27/16
 * @author mschefer
 */

#include "SEFramework/Image/Image.h"
#include "SEImplementation/Property/DetectionFramePixelValues.h"

#include "Configuration/ConfigManager.h"
#include "SEConfig/DetectionImageConfig.h"

using namespace Euclid::Configuration;

namespace SExtractor {

void DetectionFramePixelValuesTask::computeProperties(Source& source) const {
  std::vector<double> values;

  for (auto pixel_coord : source.getPixels()) {
    values.push_back(m_image->getValue(pixel_coord.m_x, pixel_coord.m_y));
  }

  source.setProperty(std::unique_ptr<DetectionFramePixelValues>(new DetectionFramePixelValues(std::move(values))));
}

DetectionFramePixelValuesTask::DetectionFramePixelValuesTask(std::shared_ptr<DetectionImage> image) : m_image(image) {
}

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



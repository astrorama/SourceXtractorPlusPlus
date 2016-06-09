/**
 * @file src/lib/PixelCentroid.cpp
 * @date 05/27/16
 * @author mschefer
 */

#include "SEFramework/Source/Source.h"

#include "SEImplementation/Property/DetectionFramePixelValues.h"
#include "SEImplementation/Property/PixelCentroid.h"

namespace SEImplementation {

void PixelCentroidTask::computeProperties(Source& source) const {
  const auto& pixel_values = source.getProperty<DetectionFramePixelValues>().getValues();

  double centroid_x = 0.0;
  double centroid_y = 0.0;
  double total_value = 0.0;

  auto i = pixel_values.begin();
  for (auto pixel_coord : source.getPixels()) {
    double value = *i++;

    total_value += value;
    centroid_x += pixel_coord.m_x * value;
    centroid_y += pixel_coord.m_y * value;
  }

  centroid_x /= total_value;
  centroid_y /= total_value;

  source.setProperty(std::unique_ptr<PixelCentroid>(new PixelCentroid(centroid_x, centroid_y)));
}

PixelCentroidTaskFactory::PixelCentroidTaskFactory() : m_pixel_centroid_task(std::make_shared<PixelCentroidTask>()) {
}

std::shared_ptr<Task> PixelCentroidTaskFactory::getTask(PropertyId property_id) {
  if (property_id == PropertyId(typeid(PixelCentroid))) {
    return m_pixel_centroid_task;
  } else {
    return nullptr;
  }
}

const std::vector<PropertyId> PixelCentroidTaskFactory::getProducedProperties() {
  return { PropertyId(typeid(PixelCentroid)) };
}


} // SEImplementation namespace




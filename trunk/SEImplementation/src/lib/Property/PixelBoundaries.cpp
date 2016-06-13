/**
 * @file src/lib/PixelBoundaries.cpp
 * @date 05/30/16
 * @author mschefer
 */


#include <climits>
#include "SEImplementation/Property/PixelBoundaries.h"

namespace SExtractor {

void PixelBoundariesTask::computeProperties(Source& source) const {
  int min_x = INT_MAX;
  int min_y = INT_MAX;
  int max_x = INT_MIN;
  int max_y = INT_MIN;

  for (auto pixel_coord : source.getPixels()) {
    min_x = std::min(min_x, pixel_coord.m_x);
    min_y = std::min(min_y, pixel_coord.m_y);
    max_x = std::max(max_x, pixel_coord.m_x);
    max_y = std::max(max_y, pixel_coord.m_y);
  }

  source.setProperty(std::unique_ptr<PixelBoundaries>(new PixelBoundaries(min_x, min_y, max_x, max_y)));
}

PixelBoundariesTaskFactory::PixelBoundariesTaskFactory()
  : m_pixel_boundaries_task(std::make_shared<PixelBoundariesTask>()) {

}

std::shared_ptr<Task> PixelBoundariesTaskFactory::getTask(PropertyId property_id) {
  if (property_id == PropertyId(typeid(PixelBoundaries))) {
    return m_pixel_boundaries_task;
  } else {
    return nullptr;
  }
}

const std::vector<PropertyId> PixelBoundariesTaskFactory::getProducedProperties() {
  return { PropertyId(typeid(PixelBoundaries)) };
}

} // SEImplementation namespace




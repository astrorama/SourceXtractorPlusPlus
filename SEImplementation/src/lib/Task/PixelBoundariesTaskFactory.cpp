/**
 * @file src/lib/Task/PixelBoundariesTaskFactory.cpp
 * @date 06/16/16
 * @author mschefer
 */

#include "SEImplementation/Property/PixelBoundaries.h"
#include "SEImplementation/Task/PixelBoundariesTask.h"
#include "SEImplementation/Task/PixelBoundariesTaskFactory.h"

namespace SExtractor {

PixelBoundariesTaskFactory::PixelBoundariesTaskFactory()
  : m_pixel_boundaries_task(std::make_shared<PixelBoundariesTask>()) {

}

std::shared_ptr<Task> PixelBoundariesTaskFactory::getTask(const PropertyId& property_id) {
  if (property_id == PropertyId::create<PixelBoundaries>()) {
    return m_pixel_boundaries_task;
  } else {
    return nullptr;
  }
}

const std::vector<PropertyId> PixelBoundariesTaskFactory::getProducedProperties() {
  return { PropertyId::create<PixelBoundaries>() };
}

} // SEImplementation namespace




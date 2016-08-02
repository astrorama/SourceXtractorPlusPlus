/**
 * @file src/lib/Task/PixelBoundariesTaskFactory.cpp
 * @date 06/16/16
 * @author mschefer
 */

#include "SEFramework/Registration/AutoRegisterer.h"

#include "SEImplementation/Property/PixelBoundaries.h"
#include "SEImplementation/Task/PixelBoundariesTask.h"
#include "SEImplementation/Task/PixelBoundariesTaskFactory.h"

namespace SExtractor {

static AutoRegisterer<PixelBoundariesTaskFactory> s_pixel_boundaries_registerer;

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

void PixelBoundariesTaskFactory::configure(Euclid::Configuration::ConfigManager&) {
}

void PixelBoundariesTaskFactory::reportConfigDependencies(Euclid::Configuration::ConfigManager&) {

}

} // SEImplementation namespace


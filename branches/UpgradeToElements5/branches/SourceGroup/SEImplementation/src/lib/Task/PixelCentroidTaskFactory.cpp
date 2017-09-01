/**
 * @file src/lib/Task/PixelCentroidTaskFactory.cpp
 * @date 06/16/16
 * @author mschefer
 */

#include "SEFramework/Registration/RegistrationManager.h"
#include "SEFramework/Registration/AutoRegisterer.h"

#include "SEImplementation/Property/PixelCentroid.h"
#include "SEImplementation/Task/PixelCentroidTask.h"
#include "SEImplementation/Task/PixelCentroidTaskFactory.h"

namespace SExtractor {

static AutoRegisterer<PixelCentroidTaskFactory, PixelCentroid> s_pixel_centroid_registerer;

PixelCentroidTaskFactory::PixelCentroidTaskFactory() : m_pixel_centroid_task(std::make_shared<PixelCentroidTask>()) {
}

std::shared_ptr<Task> PixelCentroidTaskFactory::getTask(const PropertyId& property_id) const {
  if (property_id == PropertyId::create<PixelCentroid>()) {
    return m_pixel_centroid_task;
  } else {
    return nullptr;
  }
}

const std::vector<PropertyId> PixelCentroidTaskFactory::getProducedProperties() const {
  return { PropertyId::create<PixelCentroid>() };
}

void PixelCentroidTaskFactory::configure(Euclid::Configuration::ConfigManager&) {
}


} // SEImplementation namespace


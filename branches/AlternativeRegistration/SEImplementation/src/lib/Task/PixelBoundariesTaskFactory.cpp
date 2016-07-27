/**
 * @file src/lib/Task/PixelBoundariesTaskFactory.cpp
 * @date 06/16/16
 * @author mschefer
 */

#include "SEFramework/Registration/AutoRegisterer.h"
#include "SEFramework/Output/OutputColumn.h"

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

void PixelBoundariesTaskFactory::configure(Euclid::Configuration::ConfigManager& manager) {
  OutputColumn::GetterFunction<PixelBoundaries> min_x {[](const PixelBoundaries& prop){return prop.getMin().m_x;}};
  RegistrationManager::instance().registerOutputColumn(OutputColumn("pixel_boundaries_min_x", min_x));
  OutputColumn::GetterFunction<PixelBoundaries> min_y {[](const PixelBoundaries& prop){return prop.getMin().m_y;}};
  RegistrationManager::instance().registerOutputColumn(OutputColumn("pixel_boundaries_min_y", min_y));
  OutputColumn::GetterFunction<PixelBoundaries> max_x {[](const PixelBoundaries& prop){return prop.getMax().m_x;}};
  RegistrationManager::instance().registerOutputColumn(OutputColumn("pixel_boundaries_max_x", max_x));
  OutputColumn::GetterFunction<PixelBoundaries> max_y {[](const PixelBoundaries& prop){return prop.getMax().m_y;}};
  RegistrationManager::instance().registerOutputColumn(OutputColumn("pixel_boundaries_max_y", max_y));
}

void PixelBoundariesTaskFactory::reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) {

}



} // SEImplementation namespace




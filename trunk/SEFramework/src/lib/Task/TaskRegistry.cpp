/**
 * @file src/lib/Task/TaskRegistry.cpp
 * @date 05/09/16
 * @author mschefer
 */

#include "SEFramework/Task/TaskRegistry.h"

namespace SExtractor {

void TaskRegistry::registerTaskFactory(std::unique_ptr<TaskFactory> task_factory) {
  m_factory_list.emplace_back(std::move(task_factory));
}

std::shared_ptr<Task> TaskRegistry::getTask(const PropertyId& property_id) const {
  if (m_property_factory_map.count(property_id) == 1) {
    std::size_t factory_index = m_property_factory_map.at(property_id);
    auto& factory = m_factory_list.at(factory_index);
    return factory->getTask(property_id);
  }

  return nullptr;
}

void TaskRegistry::reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) {
  for (auto& factory : m_factory_list) {
    factory->reportConfigDependencies(manager);
  }
}

void TaskRegistry::configure(Euclid::Configuration::ConfigManager& manager) {
  for (std::size_t i = 0; i < m_factory_list.size(); ++i) {
    auto& factory = m_factory_list.at(i);
    
    // First we configure the factory, so it will know which properties it produces
    factory->configure(manager);
    
    // Then we populate the property_factory_map
    for (auto& property_id : factory->getProducedProperties()) {
      if (m_property_factory_map.find(property_id) != m_property_factory_map.end()) {
        throw DuplicateFactoryException();
      }
      m_property_factory_map[property_id] = i;
    }
  }
}

} // SEFramework namespace


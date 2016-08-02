/*
 * RegistrationManager.cpp
 *
 *  Created on: Jun 16, 2016
 *      Author: mschefer
 */

#include "SEFramework/Task/TaskRegistry.h"

#include "SEFramework/Registration/RegistrationManager.h"
#include "SEFramework/Registration/OutputRegistry.h"

namespace SExtractor {

RegistrationManager::RegistrationManager()
    : m_property_id_counter(0),
      m_task_registry(std::make_shared<TaskRegistry>()) {
}

RegistrationManager::~RegistrationManager() {
}


void RegistrationManager::reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) {
  // Reports the config dependencies for every TaskFactory that was registered
  for (auto& factory : m_factory_list) {
    factory->reportConfigDependencies(manager);
  }
}

void RegistrationManager::configure(Euclid::Configuration::ConfigManager& manager) {

  for (auto& factory : m_factory_list) {
    // First we configure the factory, so it will know which properties it produces
    factory->configure(manager);
    factory->registerPropertyInstances(output_registry);

    // Then we register it in the TaskRegistry
    m_task_registry->registerTaskFactory(std::move(factory));
  }

  // clears the list, it contains only null pointers anyway
  m_factory_list.clear();
}

void RegistrationManager::registerObject(std::unique_ptr<TaskFactory> task_factory) {
  m_factory_list.emplace_back(std::move(task_factory));
}

}


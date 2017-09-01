/*
 * RegistrationManager.cpp
 *
 *  Created on: Jun 16, 2016
 *      Author: mschefer
 */

#include "SEFramework/Task/TaskFactoryRegistry.h"
#include "SEFramework/Task/TaskProvider.h"

#include "SEFramework/Registration/RegistrationManager.h"
#include "SEFramework/Registration/OutputRegistry.h"

namespace SExtractor {

RegistrationManager::RegistrationManager()
    : m_task_factory_registry(std::make_shared<TaskFactoryRegistry>()),
      m_task_provider(std::make_shared<TaskProvider>(m_task_factory_registry)) {
}

RegistrationManager::~RegistrationManager() {
}

void RegistrationManager::reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const {
  m_task_factory_registry->reportConfigDependencies(manager);
}

void RegistrationManager::configure(Euclid::Configuration::ConfigManager& manager) {
  m_task_factory_registry->configure(manager);
}

}


/*
 * TaskFactoryRegistry.cpp
 *
 *  Created on: Aug 3, 2016
 *      Author: mschefer
 */

#include "SEFramework/Task/TaskFactoryRegistry.h"
#include "SEFramework/Task/TaskFactory.h"

namespace SExtractor {

void TaskFactoryRegistry::reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const {
  // Reports the config dependencies for every TaskFactory that was registered
  for (auto& factory : m_task_factories) {
    factory->reportConfigDependencies(manager);
  }
}

void TaskFactoryRegistry::configure(Euclid::Configuration::ConfigManager& manager) {
  for (auto& factory : m_task_factories) {
    factory->configure(manager);
    factory->registerPropertyInstances(output_registry);
  }
}

}




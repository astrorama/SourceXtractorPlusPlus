/*
 * AssocModeTaskFactory.cpp
 *
 *  Created on: Mar 5, 2021
 *      Author: mschefer
 */


#include <SEImplementation/Plugin/AssocMode/AssocMode.h>
#include <iostream>
#include <sstream>

#include "SEFramework/Property/PropertyId.h"
#include "SEFramework/Task/Task.h"

#include "SEImplementation/Plugin/AssocMode/AssocModeTask.h"
#include "SEImplementation/Plugin/AssocMode/AssocModeTaskFactory.h"
#include "SEImplementation/Plugin/AssocMode/AssocModeConfig.h"


namespace SourceXtractor {

AssocModeTaskFactory::AssocModeTaskFactory() {}

void AssocModeTaskFactory::reportConfigDependencies(Euclid::Configuration::ConfigManager &manager) const {
  manager.registerConfiguration<AssocModeConfig>();
}

void AssocModeTaskFactory::configure(Euclid::Configuration::ConfigManager &manager) {
  auto config = manager.getConfiguration<AssocModeConfig>();

  m_catalog = config.getCatalog();
  m_assoc_radius = config.getAssocRadius();
  m_assoc_mode = config.getAssocMode();
}

std::shared_ptr<Task> AssocModeTaskFactory::createTask(const PropertyId &property_id) const {
  if (property_id.getTypeId() == typeid(AssocMode)) {
    return std::make_shared<AssocModeTask>(m_catalog, m_assoc_mode, m_assoc_radius);
  } else {
    return nullptr;
  }
}

}

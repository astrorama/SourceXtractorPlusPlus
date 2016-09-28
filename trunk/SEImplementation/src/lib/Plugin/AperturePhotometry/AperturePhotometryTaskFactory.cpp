/*
 * AperturePhotometryTaskFactory.cpp
 *
 *  Created on: Sep 23, 2016
 *      Author: mschefer
 */

#include <sstream>

#include "SEFramework/Property/PropertyId.h"
#include "SEFramework/Task/Task.h"

#include "SEImplementation/Plugin/AperturePhotometry/AperturePhotometry.h"
#include "SEImplementation/Plugin/AperturePhotometry/AperturePhotometryConfig.h"
#include "SEImplementation/Plugin/AperturePhotometry/AperturePhotometryTask.h"
#include "SEImplementation/Plugin/AperturePhotometry/AperturePhotometryTaskFactory.h"

namespace SExtractor {

std::shared_ptr<Task> AperturePhotometryTaskFactory::createTask(const PropertyId& property_id) const {
  if (property_id.getTypeId() == typeid(AperturePhotometry) && property_id.getIndex() < m_apertures.size()) {
    return std::make_shared<AperturePhotometryTask>(m_apertures[property_id.getIndex()], property_id.getIndex());
  } else {
    return nullptr;
  }
}

void AperturePhotometryTaskFactory::registerPropertyInstances(OutputRegistry& output_registry) {
  output_registry.registerPropertyInstances<AperturePhotometry>(m_instance_names);
}

void AperturePhotometryTaskFactory::reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const {
  manager.registerConfiguration<AperturePhotometryConfig>();
}

void AperturePhotometryTaskFactory::configure(Euclid::Configuration::ConfigManager& manager) {
  m_apertures = manager.getConfiguration<AperturePhotometryConfig>().getApertures();

  int instance_counter = 0;
  for (int i=0; i<m_apertures.size(); i++) {
    std::stringstream instance_name;
    instance_name << (instance_counter++);
    m_instance_names.emplace_back(instance_name.str());
  }
}

}


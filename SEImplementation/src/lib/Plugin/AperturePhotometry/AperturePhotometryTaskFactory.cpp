/*
 * AperturePhotometryTaskFactory.cpp
 *
 *  Created on: Sep 23, 2016
 *      Author: mschefer
 */

#include <sstream>

#include "SEFramework/Property/PropertyId.h"
#include "SEFramework/Task/Task.h"

#include "SEImplementation/Configuration/MagnitudeConfig.h"

#include "SEImplementation/Plugin/AperturePhotometry/AperturePhotometry.h"
#include "SEImplementation/Plugin/AperturePhotometry/AperturePhotometryConfig.h"
#include "SEImplementation/Plugin/AperturePhotometry/AperturePhotometryTask.h"
#include "SEImplementation/Plugin/AperturePhotometry/AperturePhotometryTaskFactory.h"

namespace SExtractor {

std::shared_ptr<Task> AperturePhotometryTaskFactory::createTask(const PropertyId& property_id) const {
  if (property_id.getTypeId() == typeid(AperturePhotometry) && property_id.getIndex() < m_apertures.size()) {
    return std::make_shared<AperturePhotometryTask>(
        std::make_shared<CircularAperture>(m_apertures[property_id.getIndex()]),
        property_id.getIndex(),
        m_magnitude_zero_point);
  } else {
    return nullptr;
  }
}

void AperturePhotometryTaskFactory::registerPropertyInstances(OutputRegistry& output_registry) {
  output_registry.registerPropertyInstances<AperturePhotometry>(m_instance_names);
}

void AperturePhotometryTaskFactory::reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const {
  manager.registerConfiguration<MagnitudeConfig>();
  manager.registerConfiguration<AperturePhotometryConfig>();
}

void AperturePhotometryTaskFactory::configure(Euclid::Configuration::ConfigManager& manager) {
  m_magnitude_zero_point = manager.getConfiguration<MagnitudeConfig>().getMagnitudeZeroPoint();
  m_apertures = manager.getConfiguration<AperturePhotometryConfig>().getApertures();

  for (unsigned int i=0; i<m_apertures.size(); i++) {
    std::stringstream instance_name;
    instance_name << i;
    m_instance_names.emplace_back(instance_name.str());
  }
}

}


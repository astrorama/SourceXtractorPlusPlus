/*
 * IsophotalFluxTaskFactory.cpp
 *
 *  Created on: Sep 28, 2016
 *      Author: mschefer
 */

#include "SEImplementation/Configuration/MagnitudeConfig.h"

#include "SEImplementation/Plugin/IsophotalFlux/IsophotalFlux.h"
#include "SEImplementation/Plugin/IsophotalFlux/IsophotalFluxTask.h"
#include "SEImplementation/Plugin/IsophotalFlux/IsophotalFluxTaskFactory.h"

namespace SExtractor {

void IsophotalFluxTaskFactory::reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const {
  manager.registerConfiguration<MagnitudeConfig>();
}

void IsophotalFluxTaskFactory::configure(Euclid::Configuration::ConfigManager& manager) {
  m_magnitude_zero_point = manager.getConfiguration<MagnitudeConfig>().getMagnitudeZeroPoint();
}

std::shared_ptr<Task> IsophotalFluxTaskFactory::createTask(const PropertyId& property_id) const {
  if (property_id == PropertyId::create<IsophotalFlux>()) {
    return std::make_shared<IsophotalFluxTask>(m_magnitude_zero_point);
  } else {
    return nullptr;
  }
}

}



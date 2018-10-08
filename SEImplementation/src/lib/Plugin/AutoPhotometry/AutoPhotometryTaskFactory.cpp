/*
 * AutoPhotometryTaskFactory.cpp
 *
 *  Created on: Jul 18, 2016
 *      Author: mkuemmel@usm.lmu.de
 */

#include <iostream>
#include <sstream>

#include "SEFramework/Property/PropertyId.h"
#include "SEFramework/Task/Task.h"


#include "SEImplementation/Configuration/WeightImageConfig.h"
#include "SEImplementation/Configuration/MagnitudeConfig.h"

#include "SEImplementation/Plugin/AutoPhotometry/AutoPhotometry.h"
#include "SEImplementation/Plugin/AutoPhotometry/AutoPhotometryConfig.h"
#include "SEImplementation/Plugin/AutoPhotometry/AutoPhotometryTask.h"
#include "SEImplementation/Plugin/AutoPhotometry/AutoPhotometryTaskFactory.h"
#include "SEImplementation/Configuration/CheckImagesConfig.h"
#include "SEImplementation/CheckImages/SourceIdCheckImage.h"


namespace SExtractor {

void AutoPhotometryTaskFactory::reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const {
  manager.registerConfiguration<MagnitudeConfig>();
  manager.registerConfiguration<AutoPhotometryConfig>();
  manager.registerConfiguration<WeightImageConfig>();
  manager.registerConfiguration<CheckImagesConfig>();
}

void AutoPhotometryTaskFactory::configure(Euclid::Configuration::ConfigManager& manager) {
  m_magnitude_zero_point = manager.getConfiguration<MagnitudeConfig>().getMagnitudeZeroPoint();
  m_kron_factor = manager.getConfiguration<AutoPhotometryConfig>().getAutoKronFactor();
  m_kron_minrad = manager.getConfiguration<AutoPhotometryConfig>().getAutoKronMinrad();
  m_symmetry_usage = manager.getConfiguration<WeightImageConfig>().symmetryUsage();
}

std::shared_ptr<Task> AutoPhotometryTaskFactory::createTask(const PropertyId& property_id) const {
  if (property_id == PropertyId::create<AutoPhotometry>()) {
    return std::make_shared<AutoPhotometryTask>(m_magnitude_zero_point, m_kron_factor, m_kron_minrad, m_symmetry_usage);
  } else {
    return nullptr;
  }
}

}


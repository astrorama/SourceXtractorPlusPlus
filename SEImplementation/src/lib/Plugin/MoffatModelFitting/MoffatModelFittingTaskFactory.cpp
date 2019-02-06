/*
 * MoffatModelFittingTaskFactory.cpp
 *
 *  Created on: May 17, 2017
 *      Author: mschefer
 */

#include <iostream>

#include "SEImplementation/Plugin/MoffatModelFitting/MoffatModelFitting.h"
#include "SEImplementation/Plugin/MoffatModelFitting/MoffatModelFittingTaskFactory.h"
#include "SEImplementation/Plugin/MoffatModelFitting/MoffatModelFittingTask.h"
#include "SEImplementation/Configuration/LegacyModelFittingConfig.h"

namespace SExtractor {

std::shared_ptr<Task> MoffatModelFittingTaskFactory::createTask(const PropertyId& property_id) const {
  if (property_id == PropertyId::create<MoffatModelFitting>()) {
    return std::make_shared<MoffatModelFittingTask>(m_max_iterations);
  } else {
    return nullptr;
  }
}

void MoffatModelFittingTaskFactory::reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const {
  manager.registerConfiguration<LegacyModelFittingConfig>();
}

void MoffatModelFittingTaskFactory::configure(Euclid::Configuration::ConfigManager& manager) {
  auto& model_fitting_config = manager.getConfiguration<LegacyModelFittingConfig>();
  m_max_iterations = model_fitting_config.getMaxIterations();
}

}


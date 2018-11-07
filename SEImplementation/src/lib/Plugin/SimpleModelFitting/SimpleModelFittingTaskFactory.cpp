/*
 * SimpleModelFittingTaskFactory.cpp
 *
 *  Created on: May 17, 2017
 *      Author: mschefer
 */

#include <iostream>

#include "SEImplementation/Plugin/SimpleModelFitting/SimpleModelFitting.h"
#include "SEImplementation/Plugin/SimpleModelFitting/SimpleModelFittingTask.h"
#include "SEImplementation/Plugin/SimpleModelFitting/SimpleModelFittingTaskFactory.h"

#include "SEImplementation/Configuration/LegacyModelFittingConfig.h"

namespace SExtractor {

std::shared_ptr<Task> SimpleModelFittingTaskFactory::createTask(const PropertyId& property_id) const {
  if (property_id == PropertyId::create<SimpleModelFitting>()) {
    return std::make_shared<SimpleModelFittingTask>(m_max_iterations);
  } else {
    return nullptr;
  }
}

void SimpleModelFittingTaskFactory::reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const {
  manager.registerConfiguration<LegacyModelFittingConfig>();
}

void SimpleModelFittingTaskFactory::configure(Euclid::Configuration::ConfigManager& manager) {
  auto& model_fitting_config = manager.getConfiguration<LegacyModelFittingConfig>();
  m_max_iterations = model_fitting_config.getMaxIterations();
}

}


/*
 * PointModelFittingTaskFactory.cpp
 *
 *  Created on: July 11, 2018
 *      Author: Alejandro Álvarez
 */

#include "SEImplementation/Configuration/ModelFittingConfig.h"
#include "SEImplementation/Plugin/PointModelFitting/PointModelFitting.h"
#include "SEImplementation/Plugin/PointModelFitting/PointModelFittingTask.h"
#include "SEImplementation/Plugin/PointModelFitting/PointModelFittingTaskFactory.h"

namespace SExtractor {

void PointModelFittingTaskFactory::reportConfigDependencies(Euclid::Configuration::ConfigManager &manager) const {
  manager.registerConfiguration<ModelFittingConfig>();
}

void PointModelFittingTaskFactory::configure(Euclid::Configuration::ConfigManager &manager) {
  auto& model_fitting_config = manager.getConfiguration<ModelFittingConfig>();
  m_max_iterations = model_fitting_config.getMaxIterations();
}

std::shared_ptr<Task> PointModelFittingTaskFactory::createTask(const SExtractor::PropertyId &property_id) const {
  if (property_id == PropertyId::create<PointModelFitting>()) {
    return std::make_shared<PointModelFittingTask>(m_max_iterations);
  } else {
    return nullptr;
  }
}

} // SExtractor
/*
 * FlexibleModelFittingTaskFactory.cpp
 *
 *  Created on: Sep 17, 2018
 *      Author: mschefer
 */

#include <iostream>

#include "SEImplementation/Plugin/FlexibleModelFitting/FlexibleModelFitting.h"
#include "SEImplementation/Plugin/FlexibleModelFitting/FlexibleModelFittingTask.h"
#include "SEImplementation/Plugin/FlexibleModelFitting/FlexibleModelFittingTaskFactory.h"

#include "SEImplementation/Configuration/ModelFittingConfig.h"
#include "SEImplementation/Configuration/MeasurementConfig.h"

namespace SExtractor {

std::shared_ptr<Task> FlexibleModelFittingTaskFactory::createTask(const PropertyId& property_id) const {
  if (property_id == PropertyId::create<FlexibleModelFitting>()) {
    return std::make_shared<FlexibleModelFittingTask>(m_max_iterations);
  } else {
    return nullptr;
  }
}

void FlexibleModelFittingTaskFactory::reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const {
  manager.registerConfiguration<ModelFittingConfig>();
}

void FlexibleModelFittingTaskFactory::configure(Euclid::Configuration::ConfigManager& manager) {
  auto& measurement_config = manager.getConfiguration<MeasurementConfig>();
  auto& model_fitting_config = manager.getConfiguration<ModelFittingConfig>();

  m_max_iterations = model_fitting_config.getMaxIterations();
}

}

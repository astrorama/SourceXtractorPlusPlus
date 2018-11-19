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

#include "SEImplementation/Configuration/PythonConfig.h"
#include "SEImplementation/Configuration/ModelFittingConfig.h"

namespace SExtractor {

std::shared_ptr<Task> FlexibleModelFittingTaskFactory::createTask(const PropertyId& property_id) const {
  if (property_id == PropertyId::create<FlexibleModelFitting>()) {
    return std::make_shared<FlexibleModelFittingTask>(m_max_iterations, m_parameters, m_frames, m_priors);
  } else {
    return nullptr;
  }
}

void FlexibleModelFittingTaskFactory::reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const {
  manager.registerConfiguration<ModelFittingConfig>();
}

void FlexibleModelFittingTaskFactory::configure(Euclid::Configuration::ConfigManager& manager) {
  std::cout << "FlexibleModelFittingTaskFactory::configure\n";
  auto& model_fitting_config = manager.getConfiguration<ModelFittingConfig>();

  for (auto const& i : model_fitting_config.getParameters()) {
    m_parameters.push_back(i.second);
  }

  m_frames = model_fitting_config.getFrames();

  // m_max_iterations = model_fitting_config.getMaxIterations();
  m_max_iterations = 99; //FIXME
}

}

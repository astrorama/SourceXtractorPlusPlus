/*
 * MultiframeModelFittingTaskFactory.cpp
 *
 *  Created on: Oct 5, 2017
 *      Author: mschefer
 */


#include <iostream>

#include "SEImplementation/Plugin/MultiframeModelFitting/MultiframeModelFitting.h"
#include "SEImplementation/Plugin/MultiframeModelFitting/MultiframeModelFittingTask.h"
#include "SEImplementation/Plugin/MultiframeModelFitting/MultiframeModelFittingTaskFactory.h"

#include "SEImplementation/Configuration/LegacyModelFittingConfig.h"
#include "SEImplementation/Configuration/MeasurementConfig.h"

namespace SExtractor {

// FIXME for now only one model fitting configuration TODO multiple from config file

std::shared_ptr<Task> MultiframeModelFittingTaskFactory::createTask(const PropertyId& property_id) const {
  if (property_id == PropertyId::create<MultiframeModelFitting>()) {
    return std::make_shared<MultiframeModelFittingTask>(m_max_iterations, m_frame_indices_per_band);
  } else {
    return nullptr;
  }
}

void MultiframeModelFittingTaskFactory::reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const {
  manager.registerConfiguration<LegacyModelFittingConfig>();
}

void MultiframeModelFittingTaskFactory::configure(Euclid::Configuration::ConfigManager& manager) {
  auto& measurement_config = manager.getConfiguration<MeasurementConfig>();

  auto& model_fitting_config = manager.getConfiguration<LegacyModelFittingConfig>();
  m_max_iterations = model_fitting_config.getMaxIterations();

  auto& groups = measurement_config.getImageGroups();

  for (auto& group : groups) {
    m_frame_indices_per_band.emplace_back();
    std::cout << "new group" << "\n";
    for (auto image_index : group->getMeasurementImageIndices()) {
      std::cout << image_index << "\n";
      m_frame_indices_per_band.back().emplace_back(image_index);
    }
  }
}

}

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

#include "SEImplementation/Configuration/ModelFittingConfig.h"
#include "SEImplementation/Configuration/MeasurementConfig.h"

namespace SExtractor {

// FIXME for now only one model fitting configuration TODO multiple from config file


std::shared_ptr<Task> MultiframeModelFittingTaskFactory::createTask(const PropertyId& property_id) const {
  if (property_id == PropertyId::create<MultiframeModelFitting>()) {
    if (m_psf == nullptr) {
      throw Elements::Exception() << "Model fitting requested but no PSF provided";
    }
    return std::make_shared<MultiframeModelFittingTask>(m_psf, m_max_iterations, m_frame_indices);
  } else {
    return nullptr;
  }
}

void MultiframeModelFittingTaskFactory::reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const {
  manager.registerConfiguration<ModelFittingConfig>();
  manager.registerConfiguration<PsfConfig>();
}

void MultiframeModelFittingTaskFactory::configure(Euclid::Configuration::ConfigManager& manager) {
  auto& model_fitting_config = manager.getConfiguration<ModelFittingConfig>();
  m_max_iterations = model_fitting_config.getMaxIterations();
  m_psf = manager.getConfiguration<PsfConfig>().getPsf();

  auto& images = manager.getConfiguration<MeasurementConfig>().getMeasurementImages();
  for (int i=0; i<std::max<int>(images.size(), 1); i++) {
    m_frame_indices.emplace_back(i);
  }
}

}



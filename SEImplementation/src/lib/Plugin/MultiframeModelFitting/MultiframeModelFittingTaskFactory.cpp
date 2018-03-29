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
    return std::make_shared<MultiframeModelFittingTask>(m_max_iterations, m_frame_indices_per_band, m_psfs);
  } else {
    return nullptr;
  }
}

void MultiframeModelFittingTaskFactory::reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const {
  manager.registerConfiguration<ModelFittingConfig>();
  manager.registerConfiguration<PsfConfig>();
}

void MultiframeModelFittingTaskFactory::configure(Euclid::Configuration::ConfigManager& manager) {
  auto& measurement_config = manager.getConfiguration<MeasurementConfig>();

  auto& model_fitting_config = manager.getConfiguration<ModelFittingConfig>();
  m_max_iterations = model_fitting_config.getMaxIterations();
  auto default_psf = manager.getConfiguration<PsfConfig>().getPsf();

  auto& groups = measurement_config.getImageGroups();
  auto& psfs = manager.getConfiguration<MeasurementConfig>().getPsfs();

  for (auto& group : groups) {
    m_frame_indices_per_band.emplace_back();
    std::cout << "new group" << "\n";
    for (auto image_index : group->getMeasurementImageIndices()) {
      std::cout << image_index << "\n";
      m_frame_indices_per_band.back().emplace_back(image_index);

      if (psfs[image_index] != nullptr) {
        m_psfs.emplace_back(psfs[image_index]);
      } else {
        if (default_psf != nullptr) {
          m_psfs.emplace_back(default_psf);
        } else {
          throw Elements::Exception() << "Model fitting requested but no PSF provided";
        }
      }
    }
  }
}

}

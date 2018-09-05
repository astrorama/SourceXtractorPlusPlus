/*
 * PsfTaskFactory.h
 *
 *  Created on: Jun 25, 2018
 *      Author: Alejandro Álvarez Ayllón
 */

#include "SEImplementation/Configuration/MeasurementConfig.h"
#include "SEImplementation/Plugin/Psf/PsfTaskFactory.h"

namespace SExtractor {

void PsfTaskFactory::reportConfigDependencies(Euclid::Configuration::ConfigManager &manager) const {
  manager.registerConfiguration<PsfPluginConfig>();
  manager.registerConfiguration<MeasurementConfig>();
}

void PsfTaskFactory::configure(Euclid::Configuration::ConfigManager &manager) {
  auto psf_config = manager.getConfiguration<PsfPluginConfig>();
  auto measurement_config = manager.getConfiguration<MeasurementConfig>();

  auto default_psf = psf_config.getPsf();
  auto psf_paths = measurement_config.getPsfsPaths();

  if (psf_paths.empty()) {
    m_vpsf.emplace_back(default_psf);
  }
  else {
    for (auto pp : psf_paths) {
      if (pp.empty()) {
        m_vpsf.emplace_back(default_psf);
      }
      else {
        m_vpsf.emplace_back(PsfPluginConfig::readPsf(pp));
      }
    }
  }

  for (auto& vpsf : m_vpsf) {
    if (!vpsf) {
      throw Elements::Exception() << "Missing PSF. Make sure every frame has a PSF, or that there is a valid default PSF";
    }
  }
}

std::shared_ptr<Task> PsfTaskFactory::createTask(const SExtractor::PropertyId &property_id) const {
  auto instance = property_id.getIndex();
  return std::make_shared<PsfTask>(instance, m_vpsf[instance]);
}

} // end SExtractor

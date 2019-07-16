/*
 * PsfTaskFactory.h
 *
 *  Created on: Jun 25, 2018
 *      Author: Alejandro Álvarez Ayllón
 */

#include "SEImplementation/Configuration/MeasurementImageConfig.h"
#include "SEImplementation/Plugin/Psf/PsfTaskFactory.h"

namespace SExtractor {

void PsfTaskFactory::reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const {
  manager.registerConfiguration<PsfPluginConfig>();
  manager.registerConfiguration<MeasurementImageConfig>();
}

void PsfTaskFactory::configure(Euclid::Configuration::ConfigManager& manager) {
  auto psf_config = manager.getConfiguration<PsfPluginConfig>();
  auto measurement_config = manager.getConfiguration<MeasurementImageConfig>();

  const auto& default_psf = psf_config.getPsf();
  const auto& psf_paths = measurement_config.getPsfsPaths();
  const auto& ids = manager.getConfiguration<MeasurementImageConfig>().getImageIds();

  assert(psf_paths.size() == ids.size());

  for (unsigned int i = 0; i < psf_paths.size(); i++) {
    if (!psf_paths[i].empty()) {
      m_vpsf[ids[i]] = PsfPluginConfig::readPsf(psf_paths[i]);
    }
    else if (default_psf) {
      m_vpsf[ids[i]] = default_psf;
    }
  }

  for (auto& vpsf : m_vpsf) {
    if (!vpsf.second) {
      throw Elements::Exception() << "Missing PSF. Make sure every frame has a PSF, or that there is a valid default PSF";
    }
  }
}

std::shared_ptr<Task> PsfTaskFactory::createTask(const SExtractor::PropertyId& property_id) const {
  auto instance = property_id.getIndex();

  if (m_vpsf.find(instance) == m_vpsf.end() || !m_vpsf.at(instance)) {
    throw Elements::Exception() << "Missing PSF. Make sure every frame has a PSF";
  }

  try {
    return std::make_shared<PsfTask>(instance, m_vpsf.at(instance));
  } catch (const std::out_of_range&) {
    return nullptr;
  }
}

} // end SExtractor

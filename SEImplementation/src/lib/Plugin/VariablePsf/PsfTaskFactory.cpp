/*
 * PsfTaskFactory.h
 *
 *  Created on: Jun 25, 2018
 *      Author: Alejandro Álvarez Ayllón
 */

#include "SEImplementation/Plugin/Psf/PsfTaskFactory.h"

namespace SExtractor {

void PsfTaskFactory::reportConfigDependencies(Euclid::Configuration::ConfigManager &manager) const {
  manager.registerConfiguration<PsfPluginConfig>();
}

void PsfTaskFactory::configure(Euclid::Configuration::ConfigManager &manager) {
  auto psf_config = manager.getConfiguration<PsfPluginConfig>();
  m_vpsf = psf_config.getPsf();
}

std::shared_ptr<Task> PsfTaskFactory::createTask(const SExtractor::PropertyId &property_id) const {
  return std::make_shared<PsfTask>(m_vpsf);
}

} // end SExtractor

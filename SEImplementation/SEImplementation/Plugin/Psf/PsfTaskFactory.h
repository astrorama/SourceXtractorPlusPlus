/*
 * PsfTaskFactory.h
 *
 *  Created on: Jun 25, 2018
 *      Author: Alejandro Álvarez Ayllón
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_PSF_PSFTASKFACTORY_H_
#define _SEIMPLEMENTATION_PLUGIN_PSF_PSFTASKFACTORY_H_

#include "SEFramework/Task/TaskFactory.h"
#include "SEImplementation/Plugin/Psf/PsfTask.h"
#include "PsfPluginConfig.h"

namespace SExtractor {

class PsfTaskFactory: public TaskFactory {
public:
  virtual ~PsfTaskFactory() = default;

  void reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const override;

  void configure(Euclid::Configuration::ConfigManager& manager) override;

  virtual std::shared_ptr<Task> createTask(const PropertyId& property_id) const override;

private:
  std::shared_ptr<VariablePsf> m_vpsf;
};

} // end SExtractor

#endif //_SEIMPLEMENTATION_PLUGIN_PSF_PSFTASKFACTORY_H_

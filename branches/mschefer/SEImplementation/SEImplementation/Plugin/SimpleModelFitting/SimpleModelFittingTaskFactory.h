/*
 * SimpleModelFittingTaskFactory.h
 *
 *  Created on: May 2, 2017
 *      Author: mschefer
 */

#ifndef SEIMPLEMENTATION_SEIMPLEMENTATION_PLUGIN_SIMPLEMODELFITTING_SIMPLEMODELFITTINGTASKFACTORY_H_
#define SEIMPLEMENTATION_SEIMPLEMENTATION_PLUGIN_SIMPLEMODELFITTING_SIMPLEMODELFITTINGTASKFACTORY_H_


#include "SEImplementation/Configuration/PsfConfig.h"
#include "SEFramework/Task/TaskFactory.h"
#include "SEImplementation/Plugin/SimpleModelFitting/SimpleModelFittingTask.h"


namespace SExtractor {

class SimpleModelFittingTaskFactory : public TaskFactory {
public:
  SimpleModelFittingTaskFactory() {}

  /// Destructor
  virtual ~SimpleModelFittingTaskFactory() = default;

  // TaskFactory implementation
  virtual std::shared_ptr<Task> createTask(const PropertyId& property_id) const;

  void reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const override;
  void configure(Euclid::Configuration::ConfigManager& manager) override;

private:
  unsigned int m_max_iterations;
  std::shared_ptr<ModelFitting::OpenCvPsf> m_psf;
};

}


#endif /* SEIMPLEMENTATION_SEIMPLEMENTATION_PLUGIN_SIMPLEMODELFITTING_SIMPLEMODELFITTINGTASKFACTORY_H_ */

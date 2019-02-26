/*
 * MoffatModelFittingTaskFactory.h
 *
 *  Created on: May 2, 2017
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_MOFFATMODELFITTING_MOFFATMODELFITTINGTASKFACTORY_H_
#define _SEIMPLEMENTATION_PLUGIN_MOFFATMODELFITTING_MOFFATMODELFITTINGTASKFACTORY_H_


#include "MoffatModelFittingTask.h"
#include "SEFramework/Task/TaskFactory.h"


namespace SExtractor {

class MoffatModelFittingTaskFactory : public TaskFactory {
public:
  MoffatModelFittingTaskFactory() {}

  /// Destructor
  virtual ~MoffatModelFittingTaskFactory() = default;

  // TaskFactory implementation
  virtual std::shared_ptr<Task> createTask(const PropertyId& property_id) const;

  void reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const override;
  void configure(Euclid::Configuration::ConfigManager& manager) override;

private:
  unsigned int m_max_iterations {0};
};

}


#endif /* _SEIMPLEMENTATION_PLUGIN_MOFFATMODELFITTING_MOFFATMODELFITTINGTASKFACTORY_H_ */

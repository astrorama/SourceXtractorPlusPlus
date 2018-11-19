/*
 * FlexibleModelFittingTaskFactory.h
 *
 *  Created on: Sep 17, 2018
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_FLEXIBLEMODELFITTING_FLEXIBLEMODELFITTINGTASKFACTORY_H_
#define _SEIMPLEMENTATION_PLUGIN_FLEXIBLEMODELFITTING_FLEXIBLEMODELFITTINGTASKFACTORY_H_

#include "SEFramework/Task/TaskFactory.h"
#include "SEImplementation/Plugin/FlexibleModelFitting/FlexibleModelFittingTask.h"

namespace SExtractor {

class FlexibleModelFittingTaskFactory : public TaskFactory {
public:
  FlexibleModelFittingTaskFactory() {}

  /// Destructor
  virtual ~FlexibleModelFittingTaskFactory() = default;

  // TaskFactory implementation
  virtual std::shared_ptr<Task> createTask(const PropertyId& property_id) const;

  void reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const override;
  void configure(Euclid::Configuration::ConfigManager& manager) override;

private:
  unsigned int m_max_iterations {0};

  std::vector<std::shared_ptr<FlexibleModelFittingParameter>> m_parameters;
  std::vector<std::shared_ptr<FlexibleModelFittingFrame>> m_frames;
  std::vector<std::shared_ptr<FlexibleModelFittingPrior>> m_priors;
};

}




#endif /* _SEIMPLEMENTATION_PLUGIN_FLEXIBLEMODELFITTING_FLEXIBLEMODELFITTINGTASKFACTORY_H_ */

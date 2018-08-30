/*
 * PointModelFittingTaskFactory.h
 *
 *  Created on: July 11, 2018
 *      Author: Alejandro Álvarez
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_POINTMODELFITTING_POINTMODELFITTINGTASKFACTORY_H_
#define _SEIMPLEMENTATION_PLUGIN_POINTMODELFITTING_POINTMODELFITTINGTASKFACTORY_H_

#include "SEFramework/Task/TaskFactory.h"

namespace SExtractor {

class PointModelFittingTaskFactory: public TaskFactory {
public:
  virtual ~PointModelFittingTaskFactory() = default;

  virtual std::shared_ptr<Task> createTask(const PropertyId& property_id) const;

  void reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const override;
  void configure(Euclid::Configuration::ConfigManager& manager) override;

private:
  unsigned int m_max_iterations {0};
};

} // SExtractor

#endif /* _SEIMPLEMENTATION_PLUGIN_POINTMODELFITTING_POINTMODELFITTINGTASKFACTORY_H_ */

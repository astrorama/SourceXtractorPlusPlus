/*
 * GroupInfoTaskFactory.h
 *
 *  Created on: 2019 M01 30
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_GROUPINFO_GROUPINFOTASKFACTORY_H_
#define _SEIMPLEMENTATION_PLUGIN_GROUPINFO_GROUPINFOTASKFACTORY_H_

#include "SEFramework/Task/TaskFactory.h"

namespace SExtractor {

/**
 * @class GroupInfoTaskFactory
 * @brief Produces GroupInfoTask
 */
class GroupInfoTaskFactory : public TaskFactory {
public:
  GroupInfoTaskFactory() {}

  /// Destructor
  virtual ~GroupInfoTaskFactory() = default;

  // TaskFactory implementation
  virtual std::shared_ptr<Task> createTask(const PropertyId& property_id) const override;

  void reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const override;
  void configure(Euclid::Configuration::ConfigManager& manager) override;

private:
};

} /* namespace SExtractor */

#endif /* _SEIMPLEMENTATION_PLUGIN_GROUPINFO_GROUPINFOTASKFACTORY_H_ */

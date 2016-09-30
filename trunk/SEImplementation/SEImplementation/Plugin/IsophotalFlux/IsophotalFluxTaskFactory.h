/**
 * @file SEImplementation/IsophotalFluxTaskFactory.h
 * @date 09/28/16
 * @author mschefer
 */

#ifndef _SEIMPLEMENTATION_TASK_ISOPHOTALFLUXTASKFACTORY_H
#define _SEIMPLEMENTATION_TASK_ISOPHOTALFLUXTASKFACTORY_H

#include "SEFramework/Task/TaskFactory.h"

namespace SExtractor {

/**
 * @class IsophotalFluxTaskFactory
 * @brief Produces IsophotalFluxTask
 *
 */
class IsophotalFluxTaskFactory : public TaskFactory {
public:
  IsophotalFluxTaskFactory() {}

  /// Destructor
  virtual ~IsophotalFluxTaskFactory() = default;

  // TaskFactory implementation
  virtual std::shared_ptr<Task> createTask(const PropertyId& property_id) const override;

  void reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const override;
  void configure(Euclid::Configuration::ConfigManager& manager) override;

private:
  SeFloat m_magnitude_zero_point;
};

} /* namespace SExtractor */


#endif

/**
 * @file SEFramework/Task/TaskFactory.h
 * @date 05/09/16
 * @author mschefer
 */

#ifndef _SEFRAMEWORK_TASK_TASKFACTORY_H
#define _SEFRAMEWORK_TASK_TASKFACTORY_H

#include <vector>
#include <memory>

#include "SEFramework/Property/PropertyId.h"
#include "SEFramework/Task/Task.h"
#include "SEFramework/Configuration/Configurable.h"
#include "SEFramework/Registration/OutputRegistry.h"

namespace SExtractor {

/**
 * @class TaskFactory
 * @brief Creates a Task for computing a given property.
 *
 */

class TaskFactory : public Configurable {

public:

  /// Destructor
  virtual ~TaskFactory() = default;

  /// Returns a Task producing a Property corresponding to the given PropertyId
  virtual std::shared_ptr<Task> getTask(const PropertyId& property_id) const = 0;

  /// Returns a vector containing all the PropertyIds of the Properties that this TaskFactory provides Tasks for.
  virtual const std::vector<PropertyId> getProducedProperties() const = 0;
  
  // Provides a default implementation of the Configurable interface that does nothing
  void reportConfigDependencies(Euclid::Configuration::ConfigManager&) const override { }
  void configure(Euclid::Configuration::ConfigManager&) override { }
  
  virtual void registerPropertyInstances(OutputRegistry&) {
    // By default do nothing
  }

private:

}; /* End of TaskFactory class */

} /* namespace SExtractor */


#endif

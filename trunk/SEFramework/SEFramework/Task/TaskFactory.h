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
#include "SEFramework/Output/OutputRegistry.h"

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
  virtual std::shared_ptr<Task> createTask(const PropertyId& property_id) const = 0;
  
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

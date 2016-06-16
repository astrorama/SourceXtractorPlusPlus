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

namespace SExtractor {

/**
 * @class TaskFactory
 * @brief Creates a Task for computing a given property
 *
 */
class TaskFactory : public Configurable {

public:

  /**
   * @brief Destructor
   */
  virtual ~TaskFactory() = default;

  virtual std::shared_ptr<Task> getTask(const PropertyId& property_id) = 0;
  virtual const std::vector<PropertyId> getProducedProperties() = 0;
  
  void reportConfigDependencies(Euclid::Configuration::ConfigManager&) override { }
  
  void configure(Euclid::Configuration::ConfigManager&) override { }


private:

}; /* End of TaskFactory class */

} /* namespace SEFramework */


#endif

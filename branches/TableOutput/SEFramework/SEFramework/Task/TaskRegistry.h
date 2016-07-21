/**
 * @file SEFramework/Task/TaskRegistry.h
 * @date 05/09/16
 * @author mschefer
 */

#ifndef _SEFRAMEWORK_TASK_TASKREGISTRY_H
#define _SEFRAMEWORK_TASK_TASKREGISTRY_H

#include <memory>
#include <unordered_map>

#include "ElementsKernel/Exception.h"

#include "SEFramework/Property/PropertyId.h"
#include "SEFramework/Task/TaskFactory.h"

namespace SExtractor {

/**
 * @class TaskRegistry
 * @brief Registry of all the TaskFactories providing the Tasks used to compute a given Property
 *
 */
class TaskRegistry {

public:

  /// Exception raised when trying to register 2 TaskFactories that produce the same PropertyId
  class DuplicateFactoryException : public Elements::Exception {
  public:
    DuplicateFactoryException() : Elements::Exception("Duplicate PropertyId in TaskRegistry") {}
  };
  
  /// Destructor
  virtual ~TaskRegistry() = default;

  /// Registers a TaskFactory that will be used to create tasks when needed
  virtual void registerTaskFactory(std::unique_ptr<TaskFactory> task_factory);

  /// Requests a Task that will be used to compute the property corresponding to the property_id
  virtual std::shared_ptr<Task> getTask(const PropertyId& property_id) const;

  /// Template version of getTask() that includes casting the returned pointer to the appropriate type
  template<class T>
  std::shared_ptr<T> getTask(const PropertyId& property_id) const {
    return std::dynamic_pointer_cast<T>(getTask(property_id));
  }

private:
    std::unordered_map<PropertyId, std::shared_ptr<TaskFactory>> m_factories;

}; /* End of TaskRegistry class */


} /* namespace SExtractor */


#endif

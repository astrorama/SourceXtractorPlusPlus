/**
 * @file SEFramework/Task/TaskProvider.h
 * @date 05/09/16
 * @author mschefer
 */

#ifndef _SEFRAMEWORK_TASK_TASKPROVIDER_H
#define _SEFRAMEWORK_TASK_TASKPROVIDER_H

#include <memory>
#include <unordered_map>

#include "ElementsKernel/Exception.h"

#include "SEFramework/Property/PropertyId.h"
#include "SEFramework/Task/TaskFactory.h"
#include "SEFramework/Task/TaskFactoryRegistry.h"

namespace SExtractor {

/**
 * @class TaskProvider
 * @brief
 *
 */
class TaskProvider {

public:

  /// Destructor
  virtual ~TaskProvider() = default;

  TaskProvider(std::shared_ptr<TaskFactoryRegistry> task_factory_registry)
    : m_task_factory_registry(task_factory_registry) {}

  /// Template version of getTask() that includes casting the returned pointer to the appropriate type
  template<class T>
  std::shared_ptr<const T> getTask(const PropertyId& property_id) const {
    return std::dynamic_pointer_cast<const T>(getTask(property_id));
  }
protected:
  /// Requests a Task that will be used to compute the property corresponding to the property_id
  virtual std::shared_ptr<const Task> getTask(const PropertyId& property_id) const;

private:
  std::shared_ptr<TaskFactoryRegistry> m_task_factory_registry;
  std::unordered_map<PropertyId, std::shared_ptr<Task>> m_tasks;

}; /* End of TaskProvider class */


} /* namespace SExtractor */


#endif

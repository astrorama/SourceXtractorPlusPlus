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
#include "SEFramework/Configuration/Configurable.h"
#include "SEFramework/Task/TaskFactory.h"

namespace SExtractor {

/**
 * @class TaskRegistry
 * @brief Used to get the Task used to create a given property
 *
 */
class TaskRegistry : public Configurable {

public:

  class DuplicateFactoryException : public Elements::Exception {
  public:
    DuplicateFactoryException() : Elements::Exception("Duplicate PropertyId in TaskRegistry") {}
  };
  
  static std::shared_ptr<TaskRegistry> getSingleton() {
    static std::shared_ptr<TaskRegistry> singleton {new TaskRegistry{}};
    return singleton;
  }

  /**
   * @brief Destructor
   */
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
  
  /// Redirects the call to all the registered task factories
  void reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) override;

  /// Redirects the call to all the registered task factories and initializes
  /// the property to factory map
  void configure(Euclid::Configuration::ConfigManager& manager) override;

private:
  
  std::vector<std::unique_ptr<TaskFactory>> m_factory_list;
  std::unordered_map<PropertyId, std::size_t> m_property_factory_map;

}; /* End of TaskRegistry class */

} /* namespace SEFramework */


#endif

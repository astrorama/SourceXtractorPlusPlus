/**
 * @file src/lib/Task/TaskRegistry.cpp
 * @date 05/09/16
 * @author mschefer
 */

#include "SEFramework/Task/TaskRegistry.h"

namespace SExtractor {

void TaskRegistry::registerTaskFactory(std::unique_ptr<TaskFactory> task_factory) {
  // List of property_ids that this factory can be used to produce
  auto& property_ids = task_factory->getProducedProperties();

  // converts the unique_ptr to shared_ptr to be able have multiple copies of the pointer
  std::shared_ptr<TaskFactory> task_factory_shared_ptr = std::move(task_factory);

  // for each property_id stores a pointer to the task_factory
  for (auto property_id : property_ids) {

    // if we already have a factory for a property_id, throw an exception
    if (m_factories.find(property_id) != m_factories.end()) {
      throw DuplicateFactoryException();
    }

    // add the factory
    m_factories[property_id] = task_factory_shared_ptr;
  }
}

std::shared_ptr<Task> TaskRegistry::getTask(const PropertyId& property_id) const {
  auto iterTaskFactory = m_factories.find(property_id);
  if (iterTaskFactory != m_factories.end()) {
    auto task_factory = iterTaskFactory->second;
    if (task_factory) {
      return task_factory->getTask(property_id);
    }
  }

  return nullptr;
}

} // SEFramework namespace


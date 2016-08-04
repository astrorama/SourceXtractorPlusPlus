/**
 * @file src/lib/Task/TaskProvider.cpp
 * @date 05/09/16
 * @author mschefer
 */

#include "SEFramework/Task/TaskProvider.h"

namespace SExtractor {

std::shared_ptr<const Task> TaskProvider::getTask(const PropertyId& property_id) const {
  // tries to find the Task for the property
  auto iterTask = m_tasks.find(property_id);

  if (iterTask != m_tasks.end()) {
    return iterTask->second;
  } else if (m_task_factory_registry != nullptr) {
    auto& task_factory = m_task_factory_registry->getFactory(property_id.getTypeId());
    auto task = task_factory.getTask(property_id);
    const_cast<TaskProvider&>(*this).m_tasks[property_id] = task;
    return task;
  } else {
    return nullptr;
  }
}

} // SEFramework namespace

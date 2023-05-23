/** Copyright © 2019 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3.0 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */
/**
 * @file src/lib/Task/TaskProvider.cpp
 * @date 05/09/16
 * @author mschefer
 */


#include <mutex>
#include "SEFramework/Task/TaskProvider.h"

namespace SourceXtractor {

namespace {
  std::mutex task_provider_mutex;
}

std::shared_ptr<const Task> TaskProvider::getTask(const PropertyId& property_id) const {
  std::lock_guard<std::mutex> lock(task_provider_mutex);

  // tries to find the Task for the property
  auto iterTask = m_tasks.find(property_id);

  if (iterTask != m_tasks.end()) {
    return iterTask->second;
  } else if (m_task_factory_registry != nullptr) {
    // Use the TaskFactoryRegistry to get the correct factory for the requested property_id
    try {
      auto& task_factory = m_task_factory_registry->getFactory(property_id.getTypeId());
      auto task = task_factory.createTask(property_id);

      // Put it in the cache
      const_cast<TaskProvider&>(*this).m_tasks[property_id] = task;

      return task;
    }
    catch (...) {
      return nullptr;
    }
  } else {
    return nullptr;
  }
}

} // SEFramework namespace

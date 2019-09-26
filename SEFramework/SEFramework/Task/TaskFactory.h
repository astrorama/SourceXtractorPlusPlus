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
  void reportConfigDependencies(Euclid::Configuration::ConfigManager&) const override {}
  void configure(Euclid::Configuration::ConfigManager&) override {}
  
  virtual void registerPropertyInstances(OutputRegistry&) {
    // By default do nothing
  }

private:

}; /* End of TaskFactory class */

} /* namespace SExtractor */


#endif

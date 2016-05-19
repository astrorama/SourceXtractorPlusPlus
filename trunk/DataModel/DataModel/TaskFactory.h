/**
 * @file DataModel/TaskFactory.h
 * @date 05/09/16
 * @author mschefer
 */

#ifndef _DATAMODEL_TASKFACTORY_H
#define _DATAMODEL_TASKFACTORY_H

#include <vector>
#include <memory>

#include "PropertyId.h"
#include "Task.h"

namespace DataModel {

/**
 * @class TaskFactory
 * @brief Creates a Task for computing a given property
 *
 */
class TaskFactory {

public:

  /**
   * @brief Destructor
   */
  virtual ~TaskFactory() = default;

  virtual std::shared_ptr<Task> getTask(PropertyId property_id) = 0;
  virtual const std::vector<PropertyId> getProducedProperties() = 0;

private:

}; /* End of TaskFactory class */

} /* namespace DataModel */


#endif

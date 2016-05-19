/**
 * @file DataModel/Task.h
 * @date 05/09/16
 * @author mschefer
 */

#ifndef _DATAMODEL_TASK_H
#define _DATAMODEL_TASK_H

namespace DataModel {

/**
 * @class Task
 * @brief Basic interface for a Task that is used to compute properties
 *
 * @details This interface is empty of content and is meant to be a common parent for different kinds of tasks
 *
 */
class Task {

public:

  /**
   * @brief Destructor
   */
  virtual ~Task() = default;


private:

}; /* End of Task class */

} /* namespace DataModel */


#endif

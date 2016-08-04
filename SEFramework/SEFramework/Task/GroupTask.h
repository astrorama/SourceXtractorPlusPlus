/**
 * @file SEFramework/Task/GroupTask.h
 * @date 05/11/16
 * @author mschefer
 */

#ifndef _SEFRAMEWORK_TASK_GROUPTASK_H
#define _SEFRAMEWORK_TASK_GROUPTASK_H

#include "SEFramework/Task/Task.h"

namespace SExtractor {

class EntangledSourceGroup;

/**
 * @class GroupTask
 * @brief A Task that acts on an EntangledSourceGroup to compute one or more properties
 *
 */
class GroupTask : public Task {

public:

  /**
   * @brief Destructor
   */
  virtual ~GroupTask() = default;

  /// Computes one or more properties for the EntangledSourceGroup and/or the Sources it contains
  virtual void computeProperties(EntangledSourceGroup& group) const = 0;


private:

}; /* End of GroupTask class */

} /* namespace SExtractor */


#endif

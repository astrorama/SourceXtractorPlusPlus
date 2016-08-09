/**
 * @file SEFramework/Task/GroupTask.h
 * @date 05/11/16
 * @author mschefer
 */

#ifndef _SEFRAMEWORK_TASK_GROUPTASK_H
#define _SEFRAMEWORK_TASK_GROUPTASK_H

#include "SEFramework/Task/Task.h"
#include "SEFramework/Source/SourceGroupInterface.h"

namespace SExtractor {

/**
 * @class GroupTask
 * @brief A Task that acts on a SourceGroup to compute one or more properties
 *
 */
class GroupTask : public Task {

public:

  /**
   * @brief Destructor
   */
  virtual ~GroupTask() = default;

  /// Computes one or more properties for the SourceGroup and/or the Sources it contains
  virtual void computeProperties(SourceGroupInterface& group) const = 0;


private:

}; /* End of GroupTask class */

} /* namespace SExtractor */


#endif

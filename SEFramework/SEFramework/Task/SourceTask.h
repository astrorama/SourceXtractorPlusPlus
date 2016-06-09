/**
 * @file SEFramework/Task/SourceTask.h
 * @date 05/09/16
 * @author mschefer
 */

#ifndef _SEFRAMEWORK_TASK_SOURCETASK_H
#define _SEFRAMEWORK_TASK_SOURCETASK_H

#include "Task.h"

namespace SEFramework {

class Source;

/**
 * @class SourceTask
 * @brief A Task that acts on a Source to compute one or more properties
 *
 */
class SourceTask : public Task {

public:

  /**
   * @brief Destructor
   */
  virtual ~SourceTask() = default;

  // Computes one or more properties for the Source
  virtual void computeProperties(Source& source) const = 0;

private:

}; /* End of SourceTask class */

} /* namespace SEFramework */


#endif

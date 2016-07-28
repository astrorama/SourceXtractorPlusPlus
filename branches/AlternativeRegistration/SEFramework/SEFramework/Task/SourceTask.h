/**
 * @file SEFramework/Task/SourceTask.h
 * @date 05/09/16
 * @author mschefer
 */

#ifndef _SEFRAMEWORK_TASK_SOURCETASK_H
#define _SEFRAMEWORK_TASK_SOURCETASK_H

#include "SEFramework/Task/Task.h"
#include "SEFramework/Source/PixelSourceInterface.h"

namespace SExtractor {

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

  /// Computes one or more properties for the Source
  virtual void computeProperties(PixelSourceInterface& source) const = 0;

private:

}; /* End of SourceTask class */

} /* namespace SExtractor */


#endif

/**
 * @file SEImplementation/Task/PixelBoundariesTask.h
 * @date 06/16/16
 * @author mschefer
 */

#ifndef _SEIMPLEMENTATION_TASK_PIXELBOUNDARIESTASK_H
#define _SEIMPLEMENTATION_TASK_PIXELBOUNDARIESTASK_H

#include "SEFramework/Source/Source.h"
#include "SEFramework/Task/SourceTask.h"

namespace SExtractor {

/**
 * @class PixelBoundariesTask
 * @brief Task to produce the PixelBoundaries Property
 *
 */
class PixelBoundariesTask : public SourceTask {

public:

  /**
   * @brief Destructor
   */
  virtual ~PixelBoundariesTask() = default;

  virtual void computeProperties(Source& source) const override;


private:

}; /* End of PixelBoundariesTask class */


} /* namespace SExtractor */


#endif

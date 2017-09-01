/**
 * @file SEImplementation/Task/PixelCentroidTask.h
 * @date 06/16/16
 * @author mschefer
 */

#ifndef _SEIMPLEMENTATION_TASK_PIXELCENTROIDTASK_H
#define _SEIMPLEMENTATION_TASK_PIXELCENTROIDTASK_H

#include "SEFramework/Task/SourceTask.h"

namespace SExtractor {

/**
 * @class PixelCentroidTask
 * @brief Task to produce the PixelCentroid Property
 *
 */

class PixelCentroidTask : public SourceTask {

public:

  /**
   * @brief Destructor
   */
  virtual ~PixelCentroidTask() = default;

  virtual void computeProperties(SourceInterface& source) const override;


private:

}; /* End of PixelCentroidTask class */


} /* namespace SExtractor */


#endif

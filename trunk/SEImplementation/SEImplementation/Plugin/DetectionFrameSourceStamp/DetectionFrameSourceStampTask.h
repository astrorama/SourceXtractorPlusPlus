/**
 * @file SEImplementation/Task/DetectionFrameSourceStampTask.h
 * @date 06/16/16
 * @author mschefer
 */

#ifndef _SEIMPLEMENTATION_TASK_DETECTIONFRAMESOURCESTAMPTASK_H
#define _SEIMPLEMENTATION_TASK_DETECTIONFRAMESOURCESTAMPTASK_H

#include "SEFramework/Image/Image.h"
#include "SEFramework/Source/SourceWithOnDemandProperties.h"
#include "SEFramework/Task/SourceTask.h"

namespace SExtractor {

/**
 * @class DetectionFrameSourceStampTask
  * @brief Task to produce the DetectionFrameSourceStamp Property
*
 */
class DetectionFrameSourceStampTask : public SourceTask {
public:

  /**
   * @brief Destructor
   */
  virtual ~DetectionFrameSourceStampTask() = default;

  /// Constructor
  DetectionFrameSourceStampTask() {}

  virtual void computeProperties(SourceInterface& source) const override;

}; /* End of DetectionFrameSourceStampTask class */

}


#endif

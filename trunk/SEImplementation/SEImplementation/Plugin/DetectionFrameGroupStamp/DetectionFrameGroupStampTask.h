/*
 * DetectionFrameGroupStampTask.h
 *
 *  Created on: May 5, 2017
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_DETECTIONFRAMEGROUPSTAMP_DETECTIONFRAMEGROUPSTAMPTASK_H_
#define _SEIMPLEMENTATION_PLUGIN_DETECTIONFRAMEGROUPSTAMP_DETECTIONFRAMEGROUPSTAMPTASK_H_


#include "SEFramework/Task/GroupTask.h"

namespace SExtractor {

/**
 * @class DetectionFrameGroupStampTask
  * @brief Task to produce the DetectionFrameGroupStamp Property
*
 */
class DetectionFrameGroupStampTask : public GroupTask {
public:

  /**
   * @brief Destructor
   */
  virtual ~DetectionFrameGroupStampTask() = default;

  /// Constructor
  DetectionFrameGroupStampTask() {}

  virtual void computeProperties(SourceGroupInterface& group) const override;

}; /* End of DetectionFrameGroupStampTask class */

}


#endif /* _SEIMPLEMENTATION_PLUGIN_DETECTIONFRAMEGROUPSTAMP_DETECTIONFRAMEGROUPSTAMPTASK_H_ */

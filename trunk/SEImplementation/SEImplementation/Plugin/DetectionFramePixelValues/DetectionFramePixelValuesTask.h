/**
 * @file SEImplementation/Task/DetectionFramePixelValuesTask.h
 * @date 06/16/16
 * @author mschefer
 */

#ifndef _SEIMPLEMENTATION_TASK_DETECTIONFRAMEPIXELVALUESTASK_H
#define _SEIMPLEMENTATION_TASK_DETECTIONFRAMEPIXELVALUESTASK_H

#include "SEFramework/Task/SourceTask.h"
#include "SEFramework/Source/SourceWithOnDemandProperties.h"
#include "SEFramework/Image/Image.h"

namespace SExtractor {

/**
 * @class DetectionFramePixelValuesTask
 * @brief Task to produce the DetectionFramePixelValues Property
 *
 */

class DetectionFramePixelValuesTask : public SourceTask {

public:

  /**
   * @brief Destructor
   */
  virtual ~DetectionFramePixelValuesTask() = default;

  DetectionFramePixelValuesTask() {}

  virtual void computeProperties(SourceInterface& source) const override;

}; /* End of DetectionFramePixelValuesTask class */



} /* namespace SExtractor */


#endif

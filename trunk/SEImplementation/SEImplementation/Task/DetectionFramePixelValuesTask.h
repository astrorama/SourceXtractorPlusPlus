/**
 * @file SEImplementation/Task/DetectionFramePixelValuesTask.h
 * @date 06/16/16
 * @author mschefer
 */

#ifndef _SEIMPLEMENTATION_TASK_DETECTIONFRAMEPIXELVALUESTASK_H
#define _SEIMPLEMENTATION_TASK_DETECTIONFRAMEPIXELVALUESTASK_H

#include "SEFramework/Task/SourceTask.h"
#include "SEFramework/Source/Source.h"
#include "SEFramework/Image/Image.h"

namespace SExtractor {

/**
 * @class DetectionFramePixelValuesTask
 * @brief
 *
 */

class DetectionFramePixelValuesTask : public SourceTask {

public:

  /**
   * @brief Destructor
   */
  virtual ~DetectionFramePixelValuesTask() = default;

  DetectionFramePixelValuesTask(std::shared_ptr<DetectionImage> image);

  virtual void computeProperties(Source& source) const override;

private:
  std::shared_ptr<DetectionImage> m_image;

}; /* End of DetectionFramePixelValuesTask class */



} /* namespace SEImplementation */


#endif

/**
 * @file SEImplementation/Task/PixelCentroidTask.h
 * @date 06/16/16
 * @author mschefer
 */

#ifndef _SEIMPLEMENTATION_TASK_MEASUREMENTFRAMEPIXELCENTROIDTASK_H
#define _SEIMPLEMENTATION_TASK_MEASUREMENTFRAMEPIXELCENTROIDTASK_H

#include "SEFramework/Task/SourceTask.h"

namespace SExtractor {

/**
 * @class MeasurementFramePixelCentroidTask
 * @brief Task to produce the PixelCentroid Property
 *
 */

class MeasurementFramePixelCentroidTask : public SourceTask {

public:

  /**
   * @brief Destructor
   */
  virtual ~MeasurementFramePixelCentroidTask() = default;

  MeasurementFramePixelCentroidTask(unsigned int instance) : m_instance(instance) {}

  virtual void computeProperties(SourceInterface& source) const override;


private:
  unsigned int m_instance;

}; /* End of MeasurementFramePixelCentroidTask class */


} /* namespace SExtractor */


#endif

/**
 * @file SEImplementation/Task/PixelCentroidTaskFactory.h
 * @date 06/16/16
 * @author mschefer
 */

#ifndef _SEIMPLEMENTATION_TASK_MEASUREMENTFRAMEPIXELCENTROIDTASKFACTORY_H
#define _SEIMPLEMENTATION_TASK_MEASUREMENTFRAMEPIXELCENTROIDTASKFACTORY_H

#include "SEFramework/Task/TaskFactory.h"

namespace SExtractor {

/**
 * @class MeasurementFramePixelCentroidTaskFactory
 * @brief Produces MeasurementFramePixelCentroidTask
 *
 */
class MeasurementFramePixelCentroidTaskFactory : public TaskFactory {
public:
  MeasurementFramePixelCentroidTaskFactory() {}

  /// Destructor
  virtual ~MeasurementFramePixelCentroidTaskFactory() = default;

  // TaskFactory implementation
  virtual std::shared_ptr<Task> createTask(const PropertyId& property_id) const override;
};

} /* namespace SExtractor */


#endif

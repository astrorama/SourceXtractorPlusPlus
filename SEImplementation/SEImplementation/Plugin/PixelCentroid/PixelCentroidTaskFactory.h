/**
 * @file SEImplementation/Task/PixelCentroidTaskFactory.h
 * @date 06/16/16
 * @author mschefer
 */

#ifndef _SEIMPLEMENTATION_TASK_PIXELCENTROIDTASKFACTORY_H
#define _SEIMPLEMENTATION_TASK_PIXELCENTROIDTASKFACTORY_H

#include "SEFramework/Task/TaskFactory.h"

namespace SExtractor {

/**
 * @class PixelCentroidTaskFactory
 * @brief Produces PixelCentroidTask
 *
 */
class PixelCentroidTaskFactory : public TaskFactory {
public:
  PixelCentroidTaskFactory() {}

  /// Destructor
  virtual ~PixelCentroidTaskFactory() = default;

  // TaskFactory implementation
  virtual std::shared_ptr<Task> createTask(const PropertyId& property_id) const override;
};


} /* namespace SExtractor */


#endif

/**
 * @file SEImplementation/Task/PixelBoundariesTaskFactory.h
 * @date 06/16/16
 * @author mschefer
 */

#ifndef _SEIMPLEMENTATION_TASK_PIXELBOUNDARIESTASKFACTORY_H
#define _SEIMPLEMENTATION_TASK_PIXELBOUNDARIESTASKFACTORY_H

#include "SEFramework/Task/TaskFactory.h"

namespace SExtractor {

/**
 * @class PixelBoundariesTaskFactory
 * @brief Produces a PixelBoundariesTask
 *
 */
class PixelBoundariesTaskFactory : public TaskFactory {

public:

  /// Destructor
  virtual ~PixelBoundariesTaskFactory() = default;

  PixelBoundariesTaskFactory() {}

  // TaskFactory implementation
  virtual std::shared_ptr<Task> createTask(const PropertyId& property_id) const override;

}; /* End of PixelBoundariesTaskFactory class */


} /* namespace SExtractor */


#endif

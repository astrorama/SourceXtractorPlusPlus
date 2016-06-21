/**
 * @file SEImplementation/Task/PixelBoundariesTaskFactory.h
 * @date 06/16/16
 * @author mschefer
 */

#ifndef _SEIMPLEMENTATION_TASK_PIXELBOUNDARIESTASKFACTORY_H
#define _SEIMPLEMENTATION_TASK_PIXELBOUNDARIESTASKFACTORY_H

#include "SEFramework/Task/TaskFactory.h"

namespace SExtractor {

class PixelBoundariesTask;

/**
 * @class PixelBoundariesTaskFactory
 * @brief Produces a PixelBoundariesTask
 *
 */
class PixelBoundariesTaskFactory : public TaskFactory {

public:

  /**
   * @brief Destructor
   */
  virtual ~PixelBoundariesTaskFactory() = default;

  PixelBoundariesTaskFactory();

  // TaskFactory implementation
  virtual std::shared_ptr<Task> getTask(const PropertyId& property_id) override;
  virtual const std::vector<PropertyId> getProducedProperties() override;

private:
  std::shared_ptr<PixelBoundariesTask> m_pixel_boundaries_task;

private:

}; /* End of PixelBoundariesTaskFactory class */


} /* namespace SExtractor */


#endif

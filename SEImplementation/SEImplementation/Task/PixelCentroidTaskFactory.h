/**
 * @file SEImplementation/Task/PixelCentroidTaskFactory.h
 * @date 06/16/16
 * @author mschefer
 */

#ifndef _SEIMPLEMENTATION_TASK_PIXELCENTROIDTASKFACTORY_H
#define _SEIMPLEMENTATION_TASK_PIXELCENTROIDTASKFACTORY_H

#include "SEFramework/Task/TaskFactory.h"

namespace SExtractor {

class PixelCentroidTask;

/**
 * @class PixelCentroidTaskFactory
 * @brief Produces PixelCentroidTask
 *
 */
class PixelCentroidTaskFactory : public TaskFactory {
public:
  PixelCentroidTaskFactory();

  /**
   * @brief Destructor
   */
  virtual ~PixelCentroidTaskFactory() = default;

  // TaskFactory implementation
  virtual std::shared_ptr<Task> getTask(const PropertyId& property_id) const override;
  virtual const std::vector<PropertyId> getProducedProperties() const override;

  // Configurable implementation
  void configure(Euclid::Configuration::ConfigManager& manager) override;

private:
  std::shared_ptr<PixelCentroidTask> m_pixel_centroid_task;
};


} /* namespace SExtractor */


#endif

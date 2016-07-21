/**
 * @file SEImplementation/Task/DetectionFramePixelValuesTaskFactory.h
 * @date 06/16/16
 * @author mschefer
 */

#ifndef _SEIMPLEMENTATION_TASK_DETECTIONFRAMEPIXELVALUESTASKFACTORY_H
#define _SEIMPLEMENTATION_TASK_DETECTIONFRAMEPIXELVALUESTASKFACTORY_H

#include <memory>
#include <vector>

#include "SEFramework/Task/TaskFactory.h"

namespace SExtractor {

class DetectionFramePixelValuesTask;

/**
 * @class DetectionFramePixelValuesTaskFactory
 * @brief TaskFactory to produce DetectionFramePixelValuesTask
 *
 */
class DetectionFramePixelValuesTaskFactory : public TaskFactory {
public:

  /// Destructor
  virtual ~DetectionFramePixelValuesTaskFactory() = default;

  // TaskFactory implementation
  virtual std::shared_ptr<Task> getTask(const PropertyId& property_id) override;
  virtual const std::vector<PropertyId> getProducedProperties() override;

  // Configurable implementation
  void configure(Euclid::Configuration::ConfigManager& manager) override;
  void reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) override;

private:
  std::shared_ptr<DetectionFramePixelValuesTask> m_detection_frame_pixel_values_task;
};

} /* namespace SExtractor */


#endif

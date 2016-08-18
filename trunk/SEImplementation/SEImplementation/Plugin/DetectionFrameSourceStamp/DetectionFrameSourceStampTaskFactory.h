/**
 * @file SEImplementation/Task/DetectionFrameSourceStampTaskFactory.h
 * @date 06/16/16
 * @author mschefer
 */

#ifndef _SEIMPLEMENTATION_TASK_DETECTIONFRAMESOURCESTAMPTASKFACTORY_H
#define _SEIMPLEMENTATION_TASK_DETECTIONFRAMESOURCESTAMPTASKFACTORY_H

#include "SEFramework/Task/TaskFactory.h"

namespace SExtractor {

class DetectionFrameSourceStampTask;

/**
 * @class DetectionFrameSourceStampTaskFactory
 * @brief Produces DetectionFrameSourceStampTask
 */

class DetectionFrameSourceStampTaskFactory : public TaskFactory {

public:

  /// Destructor
  virtual ~DetectionFrameSourceStampTaskFactory() = default;

  // TaskFactory implementation
  virtual std::shared_ptr<Task> createTask(const PropertyId& property_id) const override;

  // Configurable implementation
  void configure(Euclid::Configuration::ConfigManager& manager) override;
  void reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const override;

private:
  std::shared_ptr<DetectionImage> m_detection_image;
};

} /* namespace SExtractor */


#endif

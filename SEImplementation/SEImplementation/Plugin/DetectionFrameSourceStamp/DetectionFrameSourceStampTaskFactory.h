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
  virtual std::shared_ptr<Task> getTask(const PropertyId& property_id) const override;

  // Configurable implementation
  void configure(Euclid::Configuration::ConfigManager& manager) override;
  void reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const override;

private:
  std::shared_ptr<DetectionFrameSourceStampTask> m_detection_frame_source_stamp_task;
};

} /* namespace SExtractor */


#endif

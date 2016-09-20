/**
 * @file SEImplementation/Task/DetectionFramePixelValuesTaskFactory.h
 * @date 06/16/16
 * @author mschefer
 */

#ifndef _SEIMPLEMENTATION_TASK_DETECTIONFRAMEPIXELVALUESTASKFACTORY_H
#define _SEIMPLEMENTATION_TASK_DETECTIONFRAMEPIXELVALUESTASKFACTORY_H

#include <memory>
#include <vector>

#include "SEFramework/Image/Image.h"
#include "SEFramework/Task/TaskFactory.h"

namespace SExtractor {

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
  virtual std::shared_ptr<Task> createTask(const PropertyId& property_id) const override;
};

} /* namespace SExtractor */


#endif

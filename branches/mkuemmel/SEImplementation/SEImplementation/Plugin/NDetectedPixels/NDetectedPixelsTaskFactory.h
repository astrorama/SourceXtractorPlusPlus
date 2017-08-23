/**
 * @file SEImplementation/NDetectedPixelsTaskFactory.h
 * @date 03/14/17
 * @author mkuemmel
 */
#ifndef _SEIMPLEMENTATION_PLUGIN_NDETECTEDPIXELSTASKFACTORY_H_
#define _SEIMPLEMENTATION_PLUGIN_NDETECTEDPIXELSTASKFACTORY_H_

#include "SEFramework/Task/TaskFactory.h"
#include "SEImplementation/Plugin/NDetectedPixels/NDetectedPixelsSourceTask.h"

namespace SExtractor {
class NDetectedPixelsTaskFactory : public TaskFactory {
public:
  NDetectedPixelsTaskFactory() {}
  virtual ~NDetectedPixelsTaskFactory() = default;
  // TaskFactory implementation
  virtual std::shared_ptr<Task> createTask(const PropertyId& property_id) const {
    return std::make_shared<NDetectedPixelsSourceTask>();
  }
}; // end of NDetectedPixelsTaskFactory class
}  // namespace SExtractor

#endif /* _SEIMPLEMENTATION_PLUGIN_NDETECTEDPIXELSTASKFACTORY_H_ */

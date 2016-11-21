/*
 * WorldCentroidTaskFactory.h
 *
 *  Created on: Nov 21, 2016
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_WORLDCENTROID_WORLDCENTROIDTASKFACTORY_H_
#define _SEIMPLEMENTATION_PLUGIN_WORLDCENTROID_WORLDCENTROIDTASKFACTORY_H_

#include "SEFramework/Task/TaskFactory.h"

namespace SExtractor {

class WorldCentroidTaskFactory : public TaskFactory {
public:
  WorldCentroidTaskFactory() {}

  /// Destructor
  virtual ~WorldCentroidTaskFactory() = default;

  // TaskFactory implementation
  virtual std::shared_ptr<Task> createTask(const PropertyId& property_id) const override;
};


}

#endif

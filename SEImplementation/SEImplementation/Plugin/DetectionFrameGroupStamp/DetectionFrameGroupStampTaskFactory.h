/*
 * DetectionFrameGroupStampTaskFactory.h
 *
 *  Created on: May 5, 2017
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_DETECTIONFRAMEGROUPSTAMP_DETECTIONFRAMEGROUPSTAMPTASKFACTORY_H_
#define _SEIMPLEMENTATION_PLUGIN_DETECTIONFRAMEGROUPSTAMP_DETECTIONFRAMEGROUPSTAMPTASKFACTORY_H_


#include "SEFramework/Task/TaskFactory.h"

namespace SExtractor {

class DetectionFrameGroupStampTask;

/**
 * @class DetectionFrameGroupStampTaskFactory
 * @brief Produces DetectionFrameGroupStampTask
 */

class DetectionFrameGroupStampTaskFactory : public TaskFactory {

public:

  /// Destructor
  virtual ~DetectionFrameGroupStampTaskFactory() = default;

  // TaskFactory implementation
  virtual std::shared_ptr<Task> createTask(const PropertyId& property_id) const override;

};

} /* namespace SExtractor */





#endif /* _SEIMPLEMENTATION_PLUGIN_DETECTIONFRAMEGROUPSTAMP_DETECTIONFRAMEGROUPSTAMPTASKFACTORY_H_ */

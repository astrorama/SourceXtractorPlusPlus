/*
 * DetectionFrameGroupStampTaskFactory.cpp
 *
 *  Created on: May 5, 2017
 *      Author: mschefer
 */

#include <iostream>

#include "Configuration/ConfigManager.h"

#include "SEImplementation/Plugin/DetectionFrameGroupStamp/DetectionFrameGroupStamp.h"
#include "SEImplementation/Plugin/DetectionFrameGroupStamp/DetectionFrameGroupStampTask.h"
#include "SEImplementation/Plugin/DetectionFrameGroupStamp/DetectionFrameGroupStampTaskFactory.h"

using namespace Euclid::Configuration;

namespace SExtractor {

std::shared_ptr<Task> DetectionFrameGroupStampTaskFactory::createTask(const PropertyId& property_id) const {

  if (property_id == PropertyId::create<DetectionFrameGroupStamp>()) {
    return std::make_shared<DetectionFrameGroupStampTask>();
  } else {
    return nullptr;
  }
}

} // SEImplementation namespace





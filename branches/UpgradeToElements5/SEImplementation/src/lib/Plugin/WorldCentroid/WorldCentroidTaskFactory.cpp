/*
 * WorldCentroidTaskFactory.cpp
 *
 *  Created on: Nov 21, 2016
 *      Author: mschefer
 */

#include "SEImplementation/Plugin/WorldCentroid/WorldCentroid.h"
#include "SEImplementation/Plugin/WorldCentroid/WorldCentroidTask.h"
#include "SEImplementation/Plugin/WorldCentroid/WorldCentroidTaskFactory.h"

namespace SExtractor {

std::shared_ptr<Task> WorldCentroidTaskFactory::createTask(const PropertyId& property_id) const {
  if (property_id == PropertyId::create<WorldCentroid>()) {
    return std::make_shared<WorldCentroidTask>();
  } else {
    return nullptr;
  }
}

}


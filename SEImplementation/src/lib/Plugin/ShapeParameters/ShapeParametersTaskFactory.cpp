/*
 * ShapeParametersTaskFactory.cpp
 *
 *  Created on: Jan 27, 2017
 *      Author: mschefer
 */

#include "SEImplementation/Plugin/ShapeParameters/ShapeParameters.h"
#include "SEImplementation/Plugin/ShapeParameters/ShapeParametersTask.h"
#include "SEImplementation/Plugin/ShapeParameters/ShapeParametersTaskFactory.h"

namespace SExtractor {

std::shared_ptr<Task> ShapeParametersTaskFactory::createTask(const PropertyId& property_id) const {
  if (property_id == PropertyId::create<ShapeParameters>()) {
    return std::make_shared<ShapeParametersTask>();
  } else {
    return nullptr;
  }
}

}



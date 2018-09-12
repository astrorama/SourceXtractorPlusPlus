/*
 * KronRadiusTaskFactory.cpp
 *
 *  Created on: Sep 12, 2016
 *      Author: mkuemmel@usm.lmu.de
 */

#include <iostream>
#include <sstream>

#include "SEFramework/Property/PropertyId.h"
#include "SEFramework/Task/Task.h"

#include "SEImplementation/Plugin/KronRadius/KronRadius.h"
#include "SEImplementation/Plugin/KronRadius/KronRadiusTask.h"
#include "SEImplementation/Plugin/KronRadius/KronRadiusTaskFactory.h"

namespace SExtractor {

std::shared_ptr<Task> KronRadiusTaskFactory::createTask(const PropertyId& property_id) const {
  if (property_id == PropertyId::create<KronRadius>()) {
    return std::make_shared<KronRadiusTask>();
  } else {
    return nullptr;
  }
}

}


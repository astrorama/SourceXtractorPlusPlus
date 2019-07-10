/*
 * GroupInfoTaskFactory.cpp
 *
 *  Created on: 2019 M01 30
 *      Author: mschefer
 */

#include "SEImplementation/Plugin/GroupInfo/GroupInfo.h"
#include "SEImplementation/Plugin/GroupInfo/GroupInfoTask.h"
#include "SEImplementation/Plugin/GroupInfo/GroupInfoTaskFactory.h"

namespace SExtractor {

void GroupInfoTaskFactory::reportConfigDependencies(Euclid::Configuration::ConfigManager&) const {
}

void GroupInfoTaskFactory::configure(Euclid::Configuration::ConfigManager&) {
}

std::shared_ptr<Task> GroupInfoTaskFactory::createTask(const PropertyId& property_id) const {
  if (property_id == PropertyId::create<GroupInfo>()) {
    return std::make_shared<GroupInfoTask>();
  } else {
    return nullptr;
  }
}

}


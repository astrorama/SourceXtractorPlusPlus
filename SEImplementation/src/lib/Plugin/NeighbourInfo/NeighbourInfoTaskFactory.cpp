/*
 * NeighbourInfoTaskFactory.cpp
 *
 *  Created on: Sep 28, 2018
 *      Author: Alejandro Alvarez Ayllon
 */

#include "SEImplementation/Plugin/NeighbourInfo/NeighbourInfoTaskFactory.h"
#include "SEImplementation/Plugin/NeighbourInfo/NeighbourInfoTask.h"
#include "SEImplementation/Plugin/NeighbourInfo/NeighbourInfo.h"


namespace SExtractor {

std::shared_ptr<Task> NeighbourInfoTaskFactory::createTask(const SExtractor::PropertyId &property_id) const {
  if (property_id.getTypeId() == typeid(NeighbourInfo)) {
    return std::make_shared<NeighbourInfoTask>(property_id.getIndex());
  }
  else {
    return nullptr;
  }
}

} // end SExtractor

/*
 * NeighbourInfoTaskFactory.cpp
 *
 *  Created on: Oct 02, 2018
 *      Author: Alejandro Alvarez Ayllon
 */

#include "SEImplementation/Plugin/NeighbourInfo/NeighbourInfoTaskFactory.h"
#include "SEImplementation/Plugin/NeighbourInfo/NeighbourInfoTask.h"
#include "SEImplementation/Plugin/NeighbourInfo/NeighbourInfo.h"


namespace SExtractor {

std::shared_ptr<Task> NeighbourInfoTaskFactory::createTask(const SExtractor::PropertyId &property_id) const {
  if (property_id == PropertyId::create<NeighbourInfo>()) {
    return std::make_shared<NeighbourInfoTask>();
  }
  else {
    return nullptr;
  }
}

} // end SExtractor

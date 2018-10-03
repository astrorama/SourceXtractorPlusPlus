/*
 * DetectionNeighbourInfoTaskFactory.cpp
 *
 *  Created on: Oct 02, 2018
 *      Author: Alejandro Alvarez Ayllon
 */

#include "SEImplementation/Plugin/DetectionNeighbourInfo/DetectionNeighbourInfoTaskFactory.h"
#include "SEImplementation/Plugin/DetectionNeighbourInfo/DetectionNeighbourInfoTask.h"
#include "SEImplementation/Plugin/DetectionNeighbourInfo/DetectionNeighbourInfo.h"


namespace SExtractor {

std::shared_ptr<Task> DetectionNeighbourInfoTaskFactory::createTask(const SExtractor::PropertyId &property_id) const {
  if (property_id == PropertyId::create<DetectionNeighbourInfo>()) {
    return std::make_shared<DetectionNeighbourInfoTask>();
  }
  else {
    return nullptr;
  }
}

} // end SExtractor

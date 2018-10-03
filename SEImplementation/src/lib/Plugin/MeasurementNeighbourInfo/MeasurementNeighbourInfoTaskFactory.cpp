/*
 * MeasurementNeighbourInfoTaskFactory.cpp
 *
 *  Created on: Sep 28, 2018
 *      Author: Alejandro Alvarez Ayllon
 */

#include "SEImplementation/Plugin/MeasurementNeighbourInfo/MeasurementNeighbourInfoTaskFactory.h"
#include "SEImplementation/Plugin/MeasurementNeighbourInfo/MeasurementNeighbourInfoTask.h"
#include "SEImplementation/Plugin/MeasurementNeighbourInfo/MeasurementNeighbourInfo.h"


namespace SExtractor {

std::shared_ptr<Task> MeasurementNeighbourInfoTaskFactory::createTask(const SExtractor::PropertyId &property_id) const {
  if (property_id.getTypeId() == typeid(MeasurementNeighbourInfo)) {
    return std::make_shared<MeasurementNeighbourInfoTask>(property_id.getIndex());
  }
  else {
    return nullptr;
  }
}

} // end SExtractor

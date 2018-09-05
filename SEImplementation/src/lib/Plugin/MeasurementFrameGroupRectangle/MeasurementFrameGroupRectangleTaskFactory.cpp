/*
 * MeasurementFrameGroupRectangleTaskFactory.cpp
 *
 *  Created on: Sep 5, 2018
 *      Author: Alejandro Alvarez Ayllon
 */

#include <iostream>

#include "SEImplementation/Plugin/MeasurementFrameGroupRectangle/MeasurementFrameGroupRectangle.h"
#include "SEImplementation/Plugin/MeasurementFrameGroupRectangle/MeasurementFrameGroupRectangleTask.h"
#include "SEImplementation/Plugin/MeasurementFrameGroupRectangle/MeasurementFrameGroupRectangleTaskFactory.h"


namespace SExtractor {

std::shared_ptr<Task> MeasurementFrameGroupRectangleTaskFactory::createTask(const PropertyId& property_id) const {
  auto instance = property_id.getIndex();
  return std::make_shared<MeasurementFrameGroupRectangleTask>(instance);
}

} // SEImplementation namespace

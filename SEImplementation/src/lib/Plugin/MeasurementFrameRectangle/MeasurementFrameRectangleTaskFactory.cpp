/*
 * MeasurementFrameRectangleTaskFactory.cpp
 *
 *  Created on: Sep 24, 2018
 *      Author: Alejandro Alvarez Ayllon
 */

#include <iostream>

#include "SEImplementation/Plugin/MeasurementFrameRectangle/MeasurementFrameRectangle.h"
#include "SEImplementation/Plugin/MeasurementFrameRectangle/MeasurementFrameRectangleTask.h"
#include "SEImplementation/Plugin/MeasurementFrameRectangle/MeasurementFrameRectangleTaskFactory.h"


namespace SExtractor {

std::shared_ptr<Task> MeasurementFrameRectangleTaskFactory::createTask(const PropertyId& property_id) const {
  auto instance = property_id.getIndex();
  return std::make_shared<MeasurementFrameRectangleTask>(instance);
}

} // SEImplementation namespace

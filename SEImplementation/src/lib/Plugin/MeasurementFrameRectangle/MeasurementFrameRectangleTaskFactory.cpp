/** Copyright © 2019 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3.0 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */
/*
 * MeasurementFrameRectangleTaskFactory.cpp
 *
 *  Created on: Sep 24, 2018
 *      Author: Alejandro Alvarez Ayllon
 */

#include <iostream>

#include "SEImplementation/Configuration/DetectionFrameConfig.h"

#include "SEImplementation/Plugin/MeasurementFrameRectangle/MeasurementFrameRectangle.h"
#include "SEImplementation/Plugin/MeasurementFrameRectangle/MeasurementFrameRectangleTask.h"
#include "SEImplementation/Plugin/MeasurementFrameRectangle/MeasurementFrameRectangleTaskNoDetect.h"
#include "SEImplementation/Plugin/MeasurementFrameRectangle/MeasurementFrameRectangleTaskFactory.h"


namespace SourceXtractor {

std::shared_ptr<Task> MeasurementFrameRectangleTaskFactory::createTask(const PropertyId& property_id) const {
  auto instance = property_id.getIndex();
  if (m_no_detection_image) {
    return std::make_shared<MeasurementFrameRectangleTaskNoDetect>(instance);
  } else {
    return std::make_shared<MeasurementFrameRectangleTask>(instance);
  }

}

void MeasurementFrameRectangleTaskFactory::reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const {
  manager.registerConfiguration<DetectionFrameConfig>();
}

void MeasurementFrameRectangleTaskFactory::configure(Euclid::Configuration::ConfigManager& manager) {
  m_no_detection_image = manager.getConfiguration<DetectionFrameConfig>().getDetectionFrames().size() == 0;
}


} // SEImplementation namespace

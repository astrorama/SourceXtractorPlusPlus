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
 * MeasurementFrameTaskFactory.cpp
 *
 *  Created on: Nov 3, 2016
 *      Author: mschefer
 */

//#include <iostream>
//#include <sstream>

#include "SEImplementation/Configuration/MeasurementFrameConfig.h"

#include "SEImplementation/Plugin/MeasurementFrame/MeasurementFrame.h"
#include "SEImplementation/Plugin/MeasurementFrame/MeasurementFrameTask.h"
#include "SEImplementation/Plugin/MeasurementFrame/MeasurementFrameTaskFactory.h"

namespace SourceXtractor {

std::shared_ptr<Task> MeasurementFrameTaskFactory::createTask(const PropertyId& property_id) const {
  if (property_id.getTypeId() == PropertyId::create<MeasurementFrame>().getTypeId()) {
    auto instance = property_id.getIndex();

    try {
      return std::make_shared<MeasurementFrameTask>(instance, m_measurement_frames.at(instance));
    } catch (const std::out_of_range&) {}
  }
  return nullptr;
}

void MeasurementFrameTaskFactory::reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const {
  manager.registerConfiguration<MeasurementFrameConfig>();
}

void MeasurementFrameTaskFactory::configure(Euclid::Configuration::ConfigManager& manager) {
  m_measurement_frames = manager.getConfiguration<MeasurementFrameConfig>().getFrames();
}

}

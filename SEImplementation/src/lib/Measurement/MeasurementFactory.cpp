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
 * MeasurementFactory.cpp
 *
 *  Created on: May 17, 2018
 *      Author: mschefer
 */

#include <iostream>

#include "SEImplementation/Measurement/MeasurementFactory.h"
#include "SEImplementation/Measurement/DummyMeasurement.h"
#include "SEImplementation/Measurement/MultithreadedMeasurement.h"
#include "SEImplementation/Configuration/OutputConfig.h"
#include "SEImplementation/Configuration/MultiThreadingConfig.h"

namespace SourceXtractor {

std::unique_ptr<Measurement> MeasurementFactory::getMeasurement() const {
//  if (m_threads_nb > 0) {
//    auto source_to_row = m_output_registry->getSourceToRowConverter(m_output_properties);
//    return std::unique_ptr<Measurement>(new MultithreadedMeasurement(source_to_row, m_thread_pool, m_max_queue));
//  } else {
    return std::unique_ptr<Measurement>(new DummyMeasurement());
//  }
}

void MeasurementFactory::reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const {
  manager.registerConfiguration<OutputConfig>();
  manager.registerConfiguration<MultiThreadingConfig>();
}

void MeasurementFactory::configure(Euclid::Configuration::ConfigManager& manager) {
  m_output_properties = manager.getConfiguration<OutputConfig>().getOutputProperties();
  m_threads_nb = manager.getConfiguration<MultiThreadingConfig>().getThreadsNb();
  m_thread_pool = manager.getConfiguration<MultiThreadingConfig>().getThreadPool();
  m_max_queue = manager.getConfiguration<MultiThreadingConfig>().getMaxQueueSize();
}

}


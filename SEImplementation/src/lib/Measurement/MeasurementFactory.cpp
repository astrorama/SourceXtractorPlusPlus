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

namespace SExtractor {

std::unique_ptr<Measurement> MeasurementFactory::getMeasurement() const {
  if (m_threads_nb > 0) {
    auto source_to_row = m_output_registry->getSourceToRowConverter(m_optional_properties);
    return std::unique_ptr<Measurement>(new MultithreadedMeasurement(source_to_row, m_threads_nb));
  } else {
    return std::unique_ptr<Measurement>(new DummyMeasurement());
  }
}

void MeasurementFactory::reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const {
  manager.registerConfiguration<OutputConfig>();
  manager.registerConfiguration<MultiThreadingConfig>();
}

void MeasurementFactory::configure(Euclid::Configuration::ConfigManager& manager) {
  m_optional_properties = manager.getConfiguration<OutputConfig>().getOptionalProperties();
  m_threads_nb = manager.getConfiguration<MultiThreadingConfig>().getThreadsNb();
}

}


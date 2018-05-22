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

//#include "SEImplementation/Configuration/MeasurementConfig.h"

namespace SExtractor {

std::unique_ptr<Measurement> MeasurementFactory::getMeasurement() const {
  //return std::unique_ptr<Measurement>(new DummyMeasurement());
  return std::unique_ptr<Measurement>(new MultithreadedMeasurement(4)); // FIXME nb of threads
}

void MeasurementFactory::reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const {
  //manager.registerConfiguration<MeasurementConfig>();
}

void MeasurementFactory::configure(Euclid::Configuration::ConfigManager& manager) {
}

}


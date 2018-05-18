/*
 * MeasurementFactory.h
 *
 *  Created on: May 17, 2018
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_MEASUREMENT_MEASUREMENTFACTORY_H_
#define _SEIMPLEMENTATION_MEASUREMENT_MEASUREMENTFACTORY_H_

#include "SEFramework/Output/Output.h"
#include "SEFramework/Pipeline/Measurement.h"
#include "SEFramework/Configuration/Configurable.h"

namespace SExtractor {

/**
 * @class MeasurementFactory
 * @brief
 *
 */
class MeasurementFactory : public Configurable {

public:

  MeasurementFactory(std::shared_ptr<OutputRegistry> output_registry) : m_output_registry(output_registry) {
  }

  std::unique_ptr<Measurement> getMeasurement() const;

  // Implementation of the Configurable interface
  void configure(Euclid::Configuration::ConfigManager& manager) override;
  void reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const override;


private:
  std::shared_ptr<OutputRegistry> m_output_registry;
};

}


#endif /* _SEIMPLEMENTATION_MEASUREMENT_MEASUREMENTFACTORY_H_ */

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

  MeasurementFactory(std::shared_ptr<OutputRegistry> output_registry)
      : m_output_registry(output_registry), m_threads_nb(0) {}

  std::unique_ptr<Measurement> getMeasurement() const;

  // Implementation of the Configurable interface
  void configure(Euclid::Configuration::ConfigManager& manager) override;
  void reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const override;


private:
  std::vector<std::string> m_optional_properties;
  std::shared_ptr<OutputRegistry> m_output_registry;

  unsigned int m_threads_nb;
};

}


#endif /* _SEIMPLEMENTATION_MEASUREMENT_MEASUREMENTFACTORY_H_ */

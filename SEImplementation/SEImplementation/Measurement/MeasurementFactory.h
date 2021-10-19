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
#include "AlexandriaKernel/ThreadPool.h"

namespace SourceXtractor {

/**
 * @class MeasurementFactory
 * @brief
 *
 */
class MeasurementFactory : public Configurable {

public:

  MeasurementFactory(std::shared_ptr<OutputRegistry> output_registry)
      : m_output_registry(output_registry), m_threads_nb(0), m_max_queue(0) {}

  std::unique_ptr<Measurement> getMeasurement() const;

  // Implementation of the Configurable interface
  void configure(Euclid::Configuration::ConfigManager& manager) override;
  void reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const override;


private:
  std::vector<std::string> m_output_properties;
  std::shared_ptr<OutputRegistry> m_output_registry;
  std::shared_ptr<Euclid::ThreadPool> m_thread_pool;

  unsigned int m_threads_nb, m_max_queue;
};

}


#endif /* _SEIMPLEMENTATION_MEASUREMENT_MEASUREMENTFACTORY_H_ */

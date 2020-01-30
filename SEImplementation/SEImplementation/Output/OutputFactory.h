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
 /**
 * @file SEImplementation/Output/OutputFactory.h
 * @date 06/23/16
 * @author mschefer
 */

#ifndef _SEIMPLEMENTATION_OUTPUT_OUTPUTFACTORY_H
#define _SEIMPLEMENTATION_OUTPUT_OUTPUTFACTORY_H

#include "SEFramework/Output/Output.h"
#include "SEFramework/Configuration/Configurable.h"
#include "TableOutput.h"

namespace SourceXtractor {

/**
 * @class OutputFactory
 * @brief
 *
 */
class OutputFactory : public Configurable {

public:

  OutputFactory(std::shared_ptr<OutputRegistry> output_registry) : m_output_registry(output_registry) {
  }


  /// Destructor
  virtual ~OutputFactory() = default;

  std::unique_ptr<Output> getOutput() const;

  // Implementation of the Configurable interface
  void configure(Euclid::Configuration::ConfigManager& manager) override;
  void reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const override;

private:
  
  std::shared_ptr<OutputRegistry> m_output_registry;
  TableOutput::TableHandler m_table_handler;
  std::vector<std::string> m_output_properties;
  size_t m_flush_size;

}; /* End of OutputFactory class */

} /* namespace SourceXtractor */


#endif

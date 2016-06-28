 /**
 * @file SEImplementation/Output/OutputFactory.h
 * @date 06/23/16
 * @author mschefer
 */

#ifndef _SEIMPLEMENTATION_OUTPUT_OUTPUTFACTORY_H
#define _SEIMPLEMENTATION_OUTPUT_OUTPUTFACTORY_H

#include "SEFramework/Output/Output.h"
#include "SEFramework/Output/OutputColumn.h"
#include "SEFramework/Configuration/Configurable.h"

namespace SExtractor {

/**
 * @class OutputFactory
 * @brief
 *
 */
class OutputFactory : public Configurable {

public:

  /// Destructor
  virtual ~OutputFactory() = default;

  std::unique_ptr<Output> getOutput() const;

  // Implementation of the Configurable interface
  void configure(Euclid::Configuration::ConfigManager& manager) override;
  void reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) override;

private:
  std::vector<OutputColumn> m_output_columns;

}; /* End of OutputFactory class */

} /* namespace SExtractor */


#endif

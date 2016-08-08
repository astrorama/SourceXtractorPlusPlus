 /**
 * @file SEImplementation/Output/OutputFactory.h
 * @date 06/23/16
 * @author mschefer
 */

#ifndef _SEIMPLEMENTATION_OUTPUT_OUTPUTFACTORY_H
#define _SEIMPLEMENTATION_OUTPUT_OUTPUTFACTORY_H

#include "SEFramework/Output/Output.h"
#include "SEFramework/Configuration/Configurable.h"

namespace SExtractor {

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

}; /* End of OutputFactory class */

} /* namespace SExtractor */


#endif

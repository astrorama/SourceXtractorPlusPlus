/**
 * @file SEImplementation/Configuration/OutputConfig.h
 * @date 06/23/16
 * @author mschefer
 */

#ifndef _SEIMPLEMENTATION_CONFIGURATION_OUTPUTCONFIG_H
#define _SEIMPLEMENTATION_CONFIGURATION_OUTPUTCONFIG_H

#include "Configuration/Configuration.h"
#include "SEFramework/Output/OutputColumn.h"

namespace SExtractor {

/**
 * @class OutputConfig
 * @brief
 *
 */
class OutputConfig : public Euclid::Configuration::Configuration {

public:
  /// Destructor
  virtual ~OutputConfig() = default;

  /// Constructor
  OutputConfig(long manager_id);

  std::map<std::string, Configuration::OptionDescriptionList> getProgramOptions() override;
  
  void preInitialize(const UserValues& args) override;
  
  void initialize(const UserValues& args) override;

  const std::vector<std::string>& getOutputColumns() const {
    return m_output_columns;
  }

private:
  std::vector<std::string> m_output_columns;

}; /* End of OutputConfig class */

} /* namespace SExtractor */


#endif

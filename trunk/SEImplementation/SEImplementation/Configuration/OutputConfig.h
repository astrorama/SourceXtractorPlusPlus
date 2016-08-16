/**
 * @file SEImplementation/Configuration/OutputConfig.h
 * @date 06/23/16
 * @author mschefer
 */

#ifndef _SEIMPLEMENTATION_CONFIGURATION_OUTPUTCONFIG_H
#define _SEIMPLEMENTATION_CONFIGURATION_OUTPUTCONFIG_H

#include "Configuration/Configuration.h"
#include "SEFramework/Output/OutputRegistry.h"

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
  
  void initialize(const UserValues& args) override;
  
  void setEnabledOutputs(OutputRegistry& output_registry);
  
  std::string getOutputFile();

private:
  
  bool m_pixel_centroid;
  bool m_pixel_boundaries;
  std::string m_out_file;

}; /* End of OutputConfig class */

} /* namespace SExtractor */


#endif

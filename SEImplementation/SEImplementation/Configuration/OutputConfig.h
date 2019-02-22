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
  
  enum class OutputFileFormat {
    ASCII, FITS
  };
  
  /// Destructor
  virtual ~OutputConfig() = default;

  /// Constructor
  OutputConfig(long manager_id);

  std::map<std::string, Configuration::OptionDescriptionList> getProgramOptions() override;
  
  void preInitialize(const UserValues& args) override;

  void initialize(const UserValues& args) override;
  
  std::string getOutputFile();
  
  OutputFileFormat getOutputFileFormat();
  
  const std::vector<std::string> getOutputProperties();

private:
 
  std::string m_out_file;
  OutputFileFormat m_format;
  std::vector<std::string> m_output_properties;

}; /* End of OutputConfig class */

} /* namespace SExtractor */


#endif

/*
 * PluginConfiguration.h
 *
 *  Created on: Jul 27, 2016
 *      Author: mschefer
 */

#ifndef _SEFRAMEWORK_CONFIGURATION_PLUGINCONFIGURATION_H_
#define _SEFRAMEWORK_CONFIGURATION_PLUGINCONFIGURATION_H_

#include <boost/filesystem/path.hpp>

#include "Configuration/Configuration.h"

namespace SExtractor {

class PluginConfig :  public Euclid::Configuration::Configuration {
public:

  virtual ~PluginConfig() = default;

  PluginConfig(long manager_id);

  std::map<std::string, Configuration::OptionDescriptionList> getProgramOptions() override;
  void initialize(const UserValues& args) override;

  std::vector<boost::filesystem::path> getPluginPaths() const;

private:
  std::string m_plugin_path;
  std::vector<std::string> m_plugin_list;
};

}


#endif /* _SEFRAMEWORK_CONFIGURATION_PLUGINCONFIGURATION_H_ */

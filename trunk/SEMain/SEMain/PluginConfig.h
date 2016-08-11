/*
 * PluginConfig.h
 *
 *  Created on: Jul 27, 2016
 *      Author: mschefer
 */

#ifndef _SEMAIN_CONFIGURATION_PLUGINCONFIGURATION_H_
#define _SEMAIN_CONFIGURATION_PLUGINCONFIGURATION_H_

#include "Configuration/Configuration.h"

namespace SExtractor {

class PluginConfig :  public Euclid::Configuration::Configuration {
public:

  virtual ~PluginConfig() = default;

  PluginConfig(long manager_id);

  std::map<std::string, Configuration::OptionDescriptionList> getProgramOptions() override;
  
  void initialize(const UserValues& args) override;

  std::vector<std::string> getPluginList() const;

  std::string getPluginPath() const;


private:
  std::string m_plugin_path;
  std::vector<std::string> m_plugin_list;
};

}


#endif /* _SEMAIN_CONFIGURATION_PLUGINCONFIGURATION_H_ */

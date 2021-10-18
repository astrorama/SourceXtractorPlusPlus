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
 * PluginConfig.h
 *
 *  Created on: Jul 27, 2016
 *      Author: mschefer
 */

#ifndef _SEMAIN_CONFIGURATION_PLUGINCONFIGURATION_H_
#define _SEMAIN_CONFIGURATION_PLUGINCONFIGURATION_H_

#include "Configuration/Configuration.h"

namespace SourceXtractor {

class PluginConfig :  public Euclid::Configuration::Configuration {
public:

  virtual ~PluginConfig() = default;

  explicit PluginConfig(long manager_id);

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

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
 * @file SEImplementation/Configuration/ExternalFlagConfig.h
 * @date 06/15/16
 * @author nikoapos
 */

#ifndef _SEIMPLEMENTATION_EXTERNALFLAGCONFIG_H
#define _SEIMPLEMENTATION_EXTERNALFLAGCONFIG_H

#include <vector>
#include "Configuration/Configuration.h"
#include "SEFramework/Image/Image.h"

namespace SourceXtractor {

/**
 * @class ExternalFlagConfig
 * @brief
 *
 */
class ExternalFlagConfig : public Euclid::Configuration::Configuration {

public:
  
  enum class Type {
    OR, AND, MIN, MAX, MOST
  };
  
  using FlagInfo = std::pair<std::shared_ptr<FlagImage>, Type>;

  /**
   * @brief Destructor
   */
  virtual ~ExternalFlagConfig() = default;
  
  explicit ExternalFlagConfig(long manager_id) : Configuration(manager_id) {}

  std::map<std::string, OptionDescriptionList> getProgramOptions() override;
  
  void preInitialize(const UserValues& args) override;
  
  void initialize(const UserValues& args) override;
  
  const std::vector<std::pair<std::string, FlagInfo>>& getFlagInfoList() const;

private:
  
  std::vector<std::pair<std::string, FlagInfo>> m_flag_info_list;
  

}; /* End of ExternalFlagConfig class */

} /* namespace SourceXtractor */


#endif

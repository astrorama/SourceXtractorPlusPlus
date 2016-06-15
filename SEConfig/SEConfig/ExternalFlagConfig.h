/**
 * @file SEConfig/ExternalFlagConfig.h
 * @date 06/15/16
 * @author nikoapos
 */

#ifndef _SECONFIG_EXTERNALFLAGCONFIG_H
#define _SECONFIG_EXTERNALFLAGCONFIG_H

#include <map>
#include "Configuration/Configuration.h"
#include "SEFramework/Image/Image.h"

namespace SExtractor {

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
  
  ExternalFlagConfig(long manager_id) : Configuration(manager_id) {}

  std::map<std::string, OptionDescriptionList> getProgramOptions() override;
  
  void preInitialize(const UserValues& args) override;
  
  void initialize(const UserValues& args) override;
  
  const std::map<std::string, FlagInfo>& getFlagInfo() const;

private:
  
  std::map<std::string, FlagInfo> m_flag_info_map;
  

}; /* End of ExternalFlagConfig class */

} /* namespace SExtractor */


#endif

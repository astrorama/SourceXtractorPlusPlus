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
  
  const std::vector<std::pair<std::string, FlagInfo>>& getFlagInfoList() const;

private:
  
  std::vector<std::pair<std::string, FlagInfo>> m_flag_info_list;
  

}; /* End of ExternalFlagConfig class */

} /* namespace SExtractor */


#endif

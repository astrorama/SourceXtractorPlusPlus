/* 
 * @file BackgroundConfiguration.h
 * @author nikoapos
 */

#ifndef _SEIMPLEMENTATION_BACKGROUNDCONFIG_H
#define _SEIMPLEMENTATION_BACKGROUNDCONFIG_H

#include "Configuration/Configuration.h"
#include "SEFramework/Image/Image.h"

namespace SExtractor {

class BackgroundConfig : public Euclid::Configuration::Configuration {
  
public:
  
  BackgroundConfig(long manager_id);
  
  virtual ~BackgroundConfig() = default;

  std::map<std::string, Configuration::OptionDescriptionList> getProgramOptions() override;
  
  void initialize(const UserValues& args) override;
  
  SeFloat getBackgroundValue() const {
    return m_background_value;
  }

private:

  SeFloat m_background_value;

};

} /* namespace SExtractor */

#endif /* _SEIMPLEMENTATION_BACKGROUNDCONFIG_H */


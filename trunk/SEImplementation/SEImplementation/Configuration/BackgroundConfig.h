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

  bool isBackgroundAbsolute() const {
    return m_background_absolute;
  }

  SeFloat getThresholdValue() const {
    return m_threshold_value;
  }

  bool isThresholdAbsolute() const {
    return m_threshold_absolute;
  }

private:
  bool m_background_absolute;
  SeFloat m_background_value;

  bool m_threshold_absolute;
  SeFloat m_threshold_value;

};

} /* namespace SExtractor */

#endif /* _SEIMPLEMENTATION_BACKGROUNDCONFIG_H */


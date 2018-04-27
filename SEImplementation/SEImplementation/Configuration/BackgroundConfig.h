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
  
  SeFloat getBackgroundLevel() const {
    return m_background_level;
  }

  bool isBackgroundLevelAbsolute() const {
    return m_background_level_absolute;
  }

  SeFloat getDetectionThreshold() const {
    return m_detection_threshold;
  }

  bool isDetectionThresholdAbsolute() const {
    return m_detection_threshold_absolute;
  }

private:
  bool m_background_level_absolute;
  SeFloat m_background_level;

  bool m_detection_threshold_absolute;
  SeFloat m_detection_threshold;
};

} /* namespace SExtractor */

#endif /* _SEIMPLEMENTATION_BACKGROUNDCONFIG_H */


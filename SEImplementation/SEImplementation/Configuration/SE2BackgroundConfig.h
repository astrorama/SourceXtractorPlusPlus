/* 
 * @file BackgroundConfiguration.h
 * @author nikoapos
 */

#ifndef _SEIMPLEMENTATION_SE2BACKGROUNDCONFIG_H
#define _SEIMPLEMENTATION_SE2BACKGROUNDCONFIG_H

#include "Configuration/Configuration.h"
#include "SEFramework/Image/Image.h"

namespace SExtractor {

class SE2BackgroundConfig : public Euclid::Configuration::Configuration {
  
public:
  
  SE2BackgroundConfig(long manager_id);
  
  virtual ~SE2BackgroundConfig() = default;

  std::map<std::string, Configuration::OptionDescriptionList> getProgramOptions() override;
  
  void initialize(const UserValues& args) override;
  
  std::string getCellSize() const {
    return m_cell_size;
  }

  std::string getSmoothingBox() const {
    return m_smoothing_box;
  }

private:
  std::string m_cell_size;
  std::string m_smoothing_box;

};

} /* namespace SExtractor */

#endif /* _SEIMPLEMENTATION_SE2BACKGROUNDCONFIG_H */


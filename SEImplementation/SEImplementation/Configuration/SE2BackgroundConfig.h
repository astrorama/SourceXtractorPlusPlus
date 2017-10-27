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
  
  int getCellSize() const {
    return m_cell_size;
  }

  int getSmoothingBox() const {
    return m_smoothing_box;
  }

private:
  int m_cell_size;
  int m_smoothing_box;

};

} /* namespace SExtractor */

#endif /* _SEIMPLEMENTATION_SE2BACKGROUNDCONFIG_H */


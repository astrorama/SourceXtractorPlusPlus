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
 * @file BackgroundConfiguration.h
 * @author nikoapos
 */

#ifndef _SEIMPLEMENTATION_BACKGROUNDCONFIG_H
#define _SEIMPLEMENTATION_BACKGROUNDCONFIG_H

#include "Configuration/Configuration.h"
#include "SEFramework/Image/Image.h"

namespace SourceXtractor {

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

} /* namespace SourceXtractor */

#endif /* _SEIMPLEMENTATION_BACKGROUNDCONFIG_H */


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


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

#ifndef _SEIMPLEMENTATION_PLUGIN_VIGNETCONFIG_H
#define _SEIMPLEMENTATION_PLUGIN_VIGNETCONFIG_H

#include "Configuration/Configuration.h"
#include "SEFramework/Image/Image.h"

namespace SourceXtractor {

class VignetConfig : public Euclid::Configuration::Configuration {
  
public:
  
  VignetConfig(long manager_id);
  
  virtual ~VignetConfig() = default;

  std::map<std::string, Configuration::OptionDescriptionList> getProgramOptions() override;
  
  void initialize(const UserValues& args) override;
  
  std::string getVignetSize() const {
    return m_vignet_size;
  }

  double getVignetDefaultPixval() const {
    return m_vignet_default_pixval;
  }

private:
  std::string m_vignet_size;
  double      m_vignet_default_pixval;

};

} /* namespace SourceXtractor */

#endif /* _SEIMPLEMENTATION_PLUGIN_VIGNETCONFIG_H */


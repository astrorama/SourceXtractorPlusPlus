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
 * @file AttractorsPartitionConfig.h
 * @author nikoapos
 */

#ifndef _SEIMPLEMENTATION_ATTRACTORSPARTITIONCONFIG_H
#define _SEIMPLEMENTATION_ATTRACTORSPARTITIONCONFIG_H

#include "Configuration/Configuration.h"

namespace SourceXtractor {

class AttractorsPartitionConfig : public Euclid::Configuration::Configuration {
  
public:
  
  explicit AttractorsPartitionConfig(long manager_id);
  
  virtual ~AttractorsPartitionConfig() = default;
  
  std::map<std::string, OptionDescriptionList> getProgramOptions() override;

  void initialize(const UserValues& args) override;

};

} /* namespace SourceXtractor */

#endif /* _SEIMPLEMENTATION_ATTRACTORSPARTITIONCONFIG_H */


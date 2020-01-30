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
 * MemoryConfig.h
 *
 *  Created on: Mar 21, 2018
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_CONFIGURATION_MEMORYCONFIG_H_
#define _SEIMPLEMENTATION_CONFIGURATION_MEMORYCONFIG_H_

#include "Configuration/Configuration.h"

namespace SourceXtractor {

class MemoryConfig : public Euclid::Configuration::Configuration {
public:
  MemoryConfig(long manager_id);

  virtual ~MemoryConfig() = default;

  std::map<std::string, OptionDescriptionList> getProgramOptions() override;

  void initialize(const UserValues& args) override;

  // maximum memory allocated to ImageTiles in megabytes
  int getTileMaxMemory() const {
    return m_max_memory;
  }

  int getTileSize() const {
    return m_tile_size;
  }

private:
  int m_max_memory;
  int m_tile_size;
};


}




#endif /* _SEIMPLEMENTATION_CONFIGURATION_MEMORYCONFIG_H_ */

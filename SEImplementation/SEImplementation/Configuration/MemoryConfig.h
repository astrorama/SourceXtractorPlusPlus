/*
 * MemoryConfig.h
 *
 *  Created on: Mar 21, 2018
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_CONFIGURATION_MEMORYCONFIG_H_
#define _SEIMPLEMENTATION_CONFIGURATION_MEMORYCONFIG_H_

#include "Configuration/Configuration.h"

namespace SExtractor {

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

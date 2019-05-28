/*  
 * Copyright (C) 2012-2020 Euclid Science Ground Segment    
 *  
 * This library is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General  
 * Public License as published by the Free Software Foundation; either version 3.0 of the License, or (at your option)  
 * any later version.  
 *  
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied  
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more  
 * details.  
 *  
 * You should have received a copy of the GNU Lesser General Public License along with this library; if not, write to  
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA  
 */    

/**
 * @file SnrLevelTaskFactory.h
 *
 * @date May 27, 2019
 * @author mkuemmel@usm.lmu.de
 */
#ifndef _SEIMPLEMENTATION_PLUGIN_SNRLEVELTASKFACTORY_H_
#define _SEIMPLEMENTATION_PLUGIN_SNRLEVELTASKFACTORY_H_

#include "SEFramework/Task/TaskFactory.h"
#include "SEImplementation/Plugin/SnrLevel/SnrLevelSourceTask.h"
#include "SEImplementation/Plugin/SnrLevel/SnrLevelConfig.h"

namespace SExtractor {
class SnrLevelTaskFactory : public TaskFactory {
public:
  SnrLevelTaskFactory() {}
  virtual ~SnrLevelTaskFactory() = default;

  void reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const {
    manager.registerConfiguration<SnrLevelConfig>();
  };

  void configure(Euclid::Configuration::ConfigManager& manager) {
    auto snr_level_config = manager.getConfiguration<SnrLevelConfig>();
    m_snr_level = snr_level_config.getSnrLevel();
  };

  // TaskFactory implementation
  virtual std::shared_ptr<Task> createTask(const PropertyId& property_id) const {
    if (property_id == PropertyId::create<SnrLevel>()) {
      return std::make_shared<SnrLevelSourceTask>(m_snr_level);
    }
    else{
      return nullptr;
    }
  }

private:
  double m_snr_level;
}; // end of SnrLevelTaskFactory class
}  // namespace SExtractor
#endif /* _SEIMPLEMENTATION_PLUGIN_SNRLEVELTASKFACTORY_H_ */

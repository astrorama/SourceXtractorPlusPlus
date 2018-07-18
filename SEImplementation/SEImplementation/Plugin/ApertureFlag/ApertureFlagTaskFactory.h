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
 * @file ApertureFlagTaskFactory.h
 *
 * @date Jul 17, 2018
 * @author mkuemmel@usm.lmu.de
 */
#ifndef _SEIMPLEMENTATION_PLUGIN_APERTUREFLAGTASKFACTORY_H_
#define _SEIMPLEMENTATION_PLUGIN_APERTUREFLAGTASKFACTORY_H_

#include <iostream>

#include "SEFramework/Task/TaskFactory.h"
#include "SEImplementation/Configuration/OutputConfig.h"
#include "SEImplementation/Plugin/AperturePhotometry/AperturePhotometryConfig.h"
#include "ApertureFlagTask.h"
#include "SEImplementation/Plugin/ApertureFlag/ApertureFlagTask.h"

namespace SExtractor {
class ApertureFlagTaskFactory : public TaskFactory {
public:
  ApertureFlagTaskFactory() {}
  virtual ~ApertureFlagTaskFactory() = default;
  // TaskFactory implementation
  virtual std::shared_ptr<Task> createTask(const PropertyId& property_id) const {
    if (property_id == PropertyId::create<ApertureFlag>()) {
      return std::make_shared<ApertureFlagTask>(m_aperture_max);
    }
    else{
      return nullptr;
    }
  }

  void reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const {
    manager.registerConfiguration<AperturePhotometryConfig>();
    manager.registerConfiguration<OutputConfig>();
  }
  void configure(Euclid::Configuration::ConfigManager& manager) {
    // get the aperture size vector
    const std::vector<SeFloat>& apertures = manager.getConfiguration<AperturePhotometryConfig>().getApertures();

    // get the desired output properties
    auto& output_config = manager.getConfiguration<OutputConfig>();
    auto optional_properties = output_config.getOptionalProperties();

    if (std::find(optional_properties.begin(), optional_properties.end(), std::string("AperturePhotometry")) == optional_properties.end()){
      // if there is no aperture photometry in the output, set the radius to .0
      m_aperture_max = 0.;
    }
    else {
      // find and store the maximum aperture
      if (apertures.size()>0)
        m_aperture_max = *std::max_element(apertures.begin(), apertures.end());
      else
        m_aperture_max = 0.;
    }
  }

private:
  SeFloat m_aperture_max;
}; // end of ApertureFlagTaskFactory class
}  // namespace SExtractor
#endif /* _SEIMPLEMENTATION_PLUGIN_APERTUREFLAGTASKFACTORY_H_ */

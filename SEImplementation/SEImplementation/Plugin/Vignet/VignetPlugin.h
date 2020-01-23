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
 * @file VignetPlugin.h
 *
 * @date Jan 17, 2020
 * @author mkuemmel@usm.lmu.de
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_VIGNETPLUGIN_H_
#define _SEIMPLEMENTATION_PLUGIN_VIGNETPLUGIN_H_

#include "SEFramework/Plugin/Plugin.h"
#include "SEImplementation/Plugin/Vignet/VignetTaskFactory.h"
#include "Vignet.h"

namespace SourceXtractor {
class VignetPlugin : public Plugin {
public:
  virtual ~VignetPlugin() = default;
  virtual void registerPlugin(PluginAPI& plugin_api) {
    plugin_api.getTaskFactoryRegistry().registerTaskFactory<VignetTaskFactory, Vignet>();
    plugin_api.getOutputRegistry().registerColumnConverter<Vignet, std::vector<SeFloat>>(
            "vignet",
            [](const Vignet& prop){
              return prop.getVignet();
            },
            "[]",
            "The object vignet data"
    );
    plugin_api.getOutputRegistry().enableOutput<Vignet>("Vignet");
  }
  virtual std::string getIdString() const {
    return "Vignet";
  }
private:
}; // end of VignetPlugin class
}  // namespace SourceXtractor
#endif /* _SEIMPLEMENTATION_PLUGIN_VIGNETPLUGIN_H_ */

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
 * @file NDetectedPixelsPlugin.h
 *
 * @date Apr 27, 2018
 * @author mkuemmel@usm.lmu.de
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_NDETECTEDPIXELSPLUGIN_H_
#define _SEIMPLEMENTATION_PLUGIN_NDETECTEDPIXELSPLUGIN_H_

#include "NDetectedPixels.h"
#include "SEFramework/Plugin/Plugin.h"
#include "SEImplementation/Plugin/NDetectedPixels/NDetectedPixelsTaskFactory.h"

namespace SExtractor {
class NDetectedPixelsPlugin : public Plugin {
public:
  virtual ~NDetectedPixelsPlugin() = default;
  virtual void registerPlugin(PluginAPI& plugin_api) {
    plugin_api.getTaskFactoryRegistry().registerTaskFactory<NDetectedPixelsTaskFactory, NDetectedPixels>();
    //plugin_api.getOutputRegistry().registerColumnConverter<NDetectedPixels, int>(
    plugin_api.getOutputRegistry().registerColumnConverter<NDetectedPixels, int64_t>(
            "n_detected_pixels",
            [](const NDetectedPixels& prop){
              return prop.getNDetectedPixels();
            },
            "[]",
            "Total number of detected pixels"
    );
    plugin_api.getOutputRegistry().enableOutput<NDetectedPixels>("NDetectedPixels");
  }
  virtual std::string getIdString() const {
    return "n_detected_pixels";
  }
private:
}; // end of NDetectedPixelsPlugin class
}  // namespace SExtractor
#endif /* _SEIMPLEMENTATION_PLUGIN_NDETECTEDPIXELS_H_ */

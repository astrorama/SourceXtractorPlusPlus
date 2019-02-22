/*
 * BlendedFlagPlugin.h
 *
 *  Created on: Oct 29, 2018
 *      Author: Alejandro Alvarez Ayllon
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_BLENDEDFLAGPLUGIN_H_
#define _SEIMPLEMENTATION_PLUGIN_BLENDEDFLAGPLUGIN_H_

#include "SEFramework/Plugin/Plugin.h"
#include "BlendedFlagTaskFactory.h"

namespace SExtractor {

class BlendedFlagPlugin : public Plugin {
public:
  virtual ~BlendedFlagPlugin() = default;

  virtual void registerPlugin(PluginAPI &plugin_api) {
    plugin_api.getTaskFactoryRegistry().registerTaskFactory<BlendedFlagTaskFactory, BlendedFlag>();
  }

  virtual std::string getIdString() const {
    return "blended_flag";
  }
};

} // end SExtractor

#endif // _SEIMPLEMENTATION_PLUGIN_BLENDEDFLAGPLUGIN_H_

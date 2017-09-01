/*
 * WorldCentroidPlugin.h
 *
 *  Created on: Nov 21, 2016
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_WORLDCENTROID_WORLDCENTROIDPLUGIN_H_
#define _SEIMPLEMENTATION_PLUGIN_WORLDCENTROID_WORLDCENTROIDPLUGIN_H_

#include "SEFramework/Plugin/Plugin.h"

namespace SExtractor {

class WorldCentroidPlugin : public Plugin {

public:

  virtual ~WorldCentroidPlugin() = default;

  virtual void registerPlugin(PluginAPI& plugin_api) override;
  virtual std::string getIdString() const override;

private:

};

}

#endif

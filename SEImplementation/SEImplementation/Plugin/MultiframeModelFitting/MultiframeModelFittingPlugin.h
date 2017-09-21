/*
 * MultiframeModelFittingPlugin.h
 *
 *  Created on: Sep 12, 2017
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_MULTIFRAMEMODELFITTING_MULTIFRAMEMODELFITTINGPLUGIN_H_
#define _SEIMPLEMENTATION_PLUGIN_MULTIFRAMEMODELFITTING_MULTIFRAMEMODELFITTINGPLUGIN_H_


#include "SEFramework/Plugin/Plugin.h"

namespace SExtractor {

class MultiframeModelFittingPlugin : public Plugin {

public:

  virtual ~MultiframeModelFittingPlugin() = default;

  virtual void registerPlugin(PluginAPI& plugin_api) override;
  virtual std::string getIdString() const override;

private:

};

}



#endif /* _SEIMPLEMENTATION_PLUGIN_MULTIFRAMEMODELFITTING_MULTIFRAMEMODELFITTINGPLUGIN_H_ */

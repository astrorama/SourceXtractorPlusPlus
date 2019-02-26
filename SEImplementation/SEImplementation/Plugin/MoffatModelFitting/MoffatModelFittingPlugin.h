/*
 * MoffatModelFittingPlugin.h
 *
 *  Created on: May 2, 2017
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_MOFFATMODELFITTING_MOFFATMODELFITTINGPLUGIN_H_
#define _SEIMPLEMENTATION_PLUGIN_MOFFATMODELFITTING_MOFFATMODELFITTINGPLUGIN_H_

#include "SEFramework/Plugin/Plugin.h"

namespace SExtractor {

class MoffatModelFittingPlugin : public Plugin {

public:

  virtual ~MoffatModelFittingPlugin() = default;

  virtual void registerPlugin(PluginAPI& plugin_api) override;
  virtual std::string getIdString() const override;

private:

};

}

#endif /* _SEIMPLEMENTATION_PLUGIN_MOFFATMODELFITTING_MOFFATMODELFITTINGPLUGIN_H_ */

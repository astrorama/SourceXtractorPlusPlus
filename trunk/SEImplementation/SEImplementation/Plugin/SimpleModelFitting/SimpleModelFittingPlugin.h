/*
 * SimpleModelFittingPlugin.h
 *
 *  Created on: May 2, 2017
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_SIMPLEMODELFITTING_SIMPLEMODELFITTINGPLUGIN_H_
#define _SEIMPLEMENTATION_PLUGIN_SIMPLEMODELFITTING_SIMPLEMODELFITTINGPLUGIN_H_

#include "SEFramework/Plugin/Plugin.h"

namespace SExtractor {

class SimpleModelFittingPlugin : public Plugin {

public:

  virtual ~SimpleModelFittingPlugin() = default;

  virtual void registerPlugin(PluginAPI& plugin_api) override;
  virtual std::string getIdString() const override;

private:

};

}

#endif /* _SEIMPLEMENTATION_PLUGIN_SIMPLEMODELFITTING_SIMPLEMODELFITTINGPLUGIN_H_ */

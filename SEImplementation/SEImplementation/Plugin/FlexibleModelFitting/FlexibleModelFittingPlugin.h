/*
 * FlexibleModelFittingPlugin.h
 *
 *  Created on: Sep 17, 2018
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_FLEXIBLEMODELFITTING_FLEXIBLEMODELFITTINGPLUGIN_H_
#define _SEIMPLEMENTATION_PLUGIN_FLEXIBLEMODELFITTING_FLEXIBLEMODELFITTINGPLUGIN_H_

#include "SEFramework/Plugin/Plugin.h"

namespace SExtractor {

class FlexibleModelFittingPlugin : public Plugin {

public:

  virtual ~FlexibleModelFittingPlugin() = default;

  virtual void registerPlugin(PluginAPI& plugin_api) override;
  virtual std::string getIdString() const override;

private:

};

}


#endif /* _SEIMPLEMENTATION_PLUGIN_FLEXIBLEMODELFITTING_FLEXIBLEMODELFITTINGPLUGIN_H_ */

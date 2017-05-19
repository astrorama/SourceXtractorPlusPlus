/**
 * @file SEImplementation/IsophotalFluxPlugin.h
 * @date 09/28/16
 * @author mschefer
 */


#ifndef _SEIMPLEMENTATION_PLUGIN_ISOPHOTALFLUXPLUGIN_H_
#define _SEIMPLEMENTATION_PLUGIN_ISOPHOTALFLUXPLUGIN_H_

#include "SEFramework/Plugin/Plugin.h"

namespace SExtractor {

class IsophotalFluxPlugin : public Plugin {

public:

  /**
   * @brief Destructor
   */
  virtual ~IsophotalFluxPlugin() = default;

  virtual void registerPlugin(PluginAPI& plugin_api) override;
  virtual std::string getIdString() const override;

private:

};

}


#endif /* _SEIMPLEMENTATION_PLUGIN_ISOPHOTALFLUX_H_ */

/*
 * ExternalFlagPlugin.h
 *
 *  Created on: Aug 5, 2016
 *      Author: mschefer
 */

#ifndef SEIMPLEMENTATION_SEIMPLEMENTATION_PLUGIN_EXTERNALFLAGPLUGIN_H_
#define SEIMPLEMENTATION_SEIMPLEMENTATION_PLUGIN_EXTERNALFLAGPLUGIN_H_

#include "SEFramework/Plugin/Plugin.h"

namespace SExtractor {

class ExternalFlagPlugin : public Plugin {

public:

  /**
   * @brief Destructor
   */
  virtual ~ExternalFlagPlugin() = default;

  virtual void registerPlugin(PluginAPI& plugin_api) override;
  virtual std::string getIdString() const override;

private:

}; /* End of TestPlugin class */

}


#endif /* SEIMPLEMENTATION_SEIMPLEMENTATION_PLUGIN_EXTERNALFLAGPLUGIN_H_ */

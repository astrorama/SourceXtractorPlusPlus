/*
 * AssocModePlugin.h
 *
 *  Created on: Mar 5, 2021
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_ASSOCMODE_ASSOCMODEPLUGIN_H_
#define _SEIMPLEMENTATION_PLUGIN_ASSOCMODE_ASSOCMODEPLUGIN_H_

#include "SEFramework/Plugin/Plugin.h"

namespace SourceXtractor {

class AssocModePlugin : public Plugin {

public:

  /**
   * @brief Destructor
   */
  virtual ~AssocModePlugin() = default;

  virtual void registerPlugin(PluginAPI& plugin_api) override;
  virtual std::string getIdString() const override;

private:

};

}

#endif /* _SEIMPLEMENTATION_PLUGIN_ASSOCMODE_ASSOCMODEPLUGIN_H_ */

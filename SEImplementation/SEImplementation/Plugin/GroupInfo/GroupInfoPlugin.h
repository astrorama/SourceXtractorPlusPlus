/*
 * GroupInfoPlugin.h
 *
 *  Created on: 2019 M01 30
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_GROUPINFO_GROUPINFOPLUGIN_H_
#define _SEIMPLEMENTATION_PLUGIN_GROUPINFO_GROUPINFOPLUGIN_H_

#include "SEFramework/Plugin/Plugin.h"

namespace SExtractor {

class GroupInfoPlugin : public Plugin {

public:

  /**
   * @brief Destructor
   */
  virtual ~GroupInfoPlugin() = default;

  virtual void registerPlugin(PluginAPI& plugin_api) override;
  virtual std::string getIdString() const override;

private:

};

}


#endif /* _SEIMPLEMENTATION_PLUGIN_GROUPINFO_GROUPINFOPLUGIN_H_ */

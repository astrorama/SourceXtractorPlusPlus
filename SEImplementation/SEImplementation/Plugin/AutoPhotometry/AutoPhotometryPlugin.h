/*
 * AutoPhotometryPlugin.h
 *
 *  Created on: Jul 18, 2018
 *      Author: mkuemmel@usm.lmu.de
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_AUTOPHOTOMETRY_AUTOPHOTOMETRYPLUGIN_H_
#define _SEIMPLEMENTATION_PLUGIN_AUTOPHOTOMETRY_AUTOPHOTOMETRYPLUGIN_H_

#include "SEFramework/Plugin/Plugin.h"

namespace SExtractor {

class AutoPhotometryPlugin : public Plugin {

public:

  /**
   * @brief Destructor
   */
  virtual ~AutoPhotometryPlugin() = default;

  virtual void registerPlugin(PluginAPI& plugin_api) override;
  virtual std::string getIdString() const override;

private:

};

}


#endif /* _SEIMPLEMENTATION_PLUGIN_AUTOPHOTOMETRY_AUTOPHOTOMETRYPLUGIN_H_ */

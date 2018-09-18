/*
 * AutoPhotometryPlugin.h
 *
 *  Created on: Jul 18, 2018
 *      Author: mkuemmel@usm.lmu.de
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_KRONRADIUS_KRONRADIUSPLUGIN_H_
#define _SEIMPLEMENTATION_PLUGIN_KRONRADIUS_KRONRADIUSPLUGIN_H_

#include "SEFramework/Plugin/Plugin.h"

namespace SExtractor {

class KronRadiusPlugin : public Plugin {

public:

  /**
   * @brief Destructor
   */
  virtual ~KronRadiusPlugin() = default;

  virtual void registerPlugin(PluginAPI& plugin_api) override;
  virtual std::string getIdString() const override;

private:

};

}


#endif /* _SEIMPLEMENTATION_PLUGIN_KRONRADIUS_KRONRADIUSPLUGIN_H_ */

/*
 * PixelCentroidPlugin.h
 *
 *  Created on: Aug 5, 2016
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_PIXELCENTROIDPLUGIN_H_
#define _SEIMPLEMENTATION_PLUGIN_PIXELCENTROIDPLUGIN_H_

#include "SEFramework/Plugin/Plugin.h"

namespace SExtractor {

class PixelCentroidPlugin : public Plugin {

public:

  /**
   * @brief Destructor
   */
  virtual ~PixelCentroidPlugin() = default;

  virtual void registerPlugin(PluginAPI& plugin_api) override;
  virtual std::string getIdString() const override;

private:

};

}


#endif /* _SEIMPLEMENTATION_PLUGIN_PIXELCENTROIDPLUGIN_H_ */

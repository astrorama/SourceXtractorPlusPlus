/*
 * MeasurementFramePixelCentroidPlugin.h
 *
 *  Created on: Oct 28, 2016
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_MEASUREMENTFRAMEPIXELCENTROIDPLUGIN_H_
#define _SEIMPLEMENTATION_PLUGIN_MEASUREMENTFRAMEPIXELCENTROIDPLUGIN_H_

#include "SEFramework/Plugin/Plugin.h"

namespace SExtractor {

class MeasurementFramePixelCentroidPlugin : public Plugin {

public:

  /**
   * @brief Destructor
   */
  virtual ~MeasurementFramePixelCentroidPlugin() = default;

  virtual void registerPlugin(PluginAPI& plugin_api) override;
  virtual std::string getIdString() const override;

private:

}; /* End of TestPlugin class */

}


#endif /* _SEIMPLEMENTATION_PLUGIN_MEASUREMENTFRAMEPIXELCENTROIDPLUGIN_H_ */

/*
 * DetectionFramePixelValuesPlugin.h
 *
 *  Created on: Aug 5, 2016
 *      Author: mschefer
 */

#ifndef SEIMPLEMENTATION_SEIMPLEMENTATION_PLUGIN_DETECTIONFRAMEPIXELVALUESPLUGIN_H_
#define SEIMPLEMENTATION_SEIMPLEMENTATION_PLUGIN_DETECTIONFRAMEPIXELVALUESPLUGIN_H_

#include "SEFramework/Plugin/Plugin.h"

namespace SExtractor {

class DetectionFramePixelValuesPlugin : public Plugin {

public:

  /**
   * @brief Destructor
   */
  virtual ~DetectionFramePixelValuesPlugin() = default;

  virtual void registerPlugin(PluginAPI& plugin_api) override;
  virtual std::string getIdString() const override;

private:

}; /* End of TestPlugin class */

}


#endif /* SEIMPLEMENTATION_SEIMPLEMENTATION_PLUGIN_DETECTIONFRAMEPIXELVALUESPLUGIN_H_ */

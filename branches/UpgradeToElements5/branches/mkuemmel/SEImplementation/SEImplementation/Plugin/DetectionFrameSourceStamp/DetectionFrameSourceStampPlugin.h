/*
 * DetectionFrameSourceStampPlugin.h
 *
 *  Created on: Aug 5, 2016
 *      Author: mschefer
 */

#ifndef SEIMPLEMENTATION_SEIMPLEMENTATION_PLUGIN_DETECTIONFRAMESOURCESTAMPPLUGIN_H_
#define SEIMPLEMENTATION_SEIMPLEMENTATION_PLUGIN_DETECTIONFRAMESOURCESTAMPPLUGIN_H_

#include "SEFramework/Plugin/Plugin.h"

namespace SExtractor {

class DetectionFrameSourceStampPlugin : public Plugin {

public:

  /**
   * @brief Destructor
   */
  virtual ~DetectionFrameSourceStampPlugin() = default;

  virtual void registerPlugin(PluginAPI& plugin_api) override;
  virtual std::string getIdString() const override;

private:

}; /* End of DetectionFrameSourceStampPlugin class */

}


#endif /* SEIMPLEMENTATION_SEIMPLEMENTATION_PLUGIN_DETECTIONFRAMESOURCESTAMPPLUGIN_H_ */

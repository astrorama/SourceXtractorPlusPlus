/*
 * DetectionFrameGroupStampPlugin.h
 *
 *  Created on: May 5, 2017
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_DETECTIONFRAMEGROUPSTAMP_DETECTIONFRAMEGROUPSTAMPPLUGIN_H_
#define _SEIMPLEMENTATION_PLUGIN_DETECTIONFRAMEGROUPSTAMP_DETECTIONFRAMEGROUPSTAMPPLUGIN_H_

#include "SEFramework/Plugin/Plugin.h"

namespace SExtractor {

class DetectionFrameGroupStampPlugin : public Plugin {

public:

  /**
   * @brief Destructor
   */
  virtual ~DetectionFrameGroupStampPlugin() = default;

  virtual void registerPlugin(PluginAPI& plugin_api) override;
  virtual std::string getIdString() const override;

private:

}; /* End of DetectionFrameGroupStampPlugin class */

}

#endif /* _SEIMPLEMENTATION_PLUGIN_DETECTIONFRAMEGROUPSTAMP_DETECTIONFRAMEGROUPSTAMPPLUGIN_H_ */

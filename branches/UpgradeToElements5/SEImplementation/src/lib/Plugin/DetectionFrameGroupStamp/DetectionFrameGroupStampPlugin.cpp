/*
 * DetectionFrameGroupStampPlugin.cpp
 *
 *  Created on: May 5, 2017
 *      Author: mschefer
 */

#include "SEFramework/Plugin/StaticPlugin.h"

#include "SEImplementation/Plugin/DetectionFrameGroupStamp/DetectionFrameGroupStamp.h"
#include "SEImplementation/Plugin/DetectionFrameGroupStamp/DetectionFrameGroupStampTaskFactory.h"

#include "SEImplementation/Plugin/DetectionFrameGroupStamp/DetectionFrameGroupStampPlugin.h"

namespace SExtractor {

static StaticPlugin<DetectionFrameGroupStampPlugin> detection_frame_group_stamp_plugin;

void DetectionFrameGroupStampPlugin::registerPlugin(PluginAPI& plugin_api) {
  plugin_api.getTaskFactoryRegistry().registerTaskFactory<DetectionFrameGroupStampTaskFactory, DetectionFrameGroupStamp>();
}

std::string DetectionFrameGroupStampPlugin::getIdString() const {
  return "DetectionFrameGroupStamp";
}

}



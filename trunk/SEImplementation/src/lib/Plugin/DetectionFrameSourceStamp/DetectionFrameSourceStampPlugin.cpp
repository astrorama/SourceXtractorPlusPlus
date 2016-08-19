/*
 * DetectionFrameSourceStampPlugin.cpp
 *
 *  Created on: Aug 5, 2016
 *      Author: mschefer
 */

#include "SEFramework/Plugin/StaticPlugin.h"

#include "SEImplementation/Plugin/DetectionFrameSourceStamp/DetectionFrameSourceStamp.h"
#include "SEImplementation/Plugin/DetectionFrameSourceStamp/DetectionFrameSourceStampTaskFactory.h"

#include "SEImplementation/Plugin/DetectionFrameSourceStamp/DetectionFrameSourceStampPlugin.h"

namespace SExtractor {

static StaticPlugin<DetectionFrameSourceStampPlugin> detection_frame_source_stamp_plugin;

void DetectionFrameSourceStampPlugin::registerPlugin(PluginAPI& plugin_api) {
  plugin_api.getTaskFactoryRegistry().registerTaskFactory<DetectionFrameSourceStampTaskFactory, DetectionFrameSourceStamp>();
}

std::string DetectionFrameSourceStampPlugin::getIdString() const {
  return "";
}

}




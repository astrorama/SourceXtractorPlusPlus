/*
 * DetectionFrameSourceStampPlugin.cpp
 *
 *  Created on: Aug 5, 2016
 *      Author: mschefer
 */

#include "SEFramework/Plugin/StaticPlugin.h"

#include "SEImplementation/Property/DetectionFrameSourceStamp.h"
#include "SEImplementation/Task/DetectionFrameSourceStampTaskFactory.h"

#include "SEImplementation/Plugin/DetectionFrameSourceStampPlugin.h"

namespace SExtractor {

static StaticPlugin<DetectionFrameSourceStampPlugin> detection_frame_source_stamp_plugin;

void DetectionFrameSourceStampPlugin::registerPlugin(PluginAPI& plugin_api) {
  plugin_api.getTaskFactoryRegistry().registerTaskFactory<DetectionFrameSourceStamp>(
      std::unique_ptr<TaskFactory>(new DetectionFrameSourceStampTaskFactory));
}

std::string DetectionFrameSourceStampPlugin::getIdString() const {
  return "";
}

}




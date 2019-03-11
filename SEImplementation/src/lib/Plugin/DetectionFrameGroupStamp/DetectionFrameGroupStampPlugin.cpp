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
#include "SEImplementation/Image/ImageInterfaceTraits.h"

namespace SExtractor {

static StaticPlugin<DetectionFrameGroupStampPlugin> detection_frame_group_stamp_plugin;

void DetectionFrameGroupStampPlugin::registerPlugin(PluginAPI& plugin_api) {
  plugin_api.getTaskFactoryRegistry().registerTaskFactory<DetectionFrameGroupStampTaskFactory, DetectionFrameGroupStamp>();

  plugin_api.getOutputRegistry().registerColumnConverter<DetectionFrameGroupStamp, int>(
    "group_stamp_top",
    [](const DetectionFrameGroupStamp &d) {
      return d.getTopLeft().m_y;
    },
    "[pixel]",
    "Maximum y-coordinate of the detection group"
  );
  plugin_api.getOutputRegistry().registerColumnConverter<DetectionFrameGroupStamp, int>(
    "group_stamp_left",
    [](const DetectionFrameGroupStamp &d) {
      return d.getTopLeft().m_x;
    },
    "[pixel]",
    "Minimum x-coordinate of the detection group"
  );
  plugin_api.getOutputRegistry().registerColumnConverter<DetectionFrameGroupStamp, int>(
    "group_stamp_width",
    [](const DetectionFrameGroupStamp &d) {
      return d.getStamp().getWidth();
    },
    "[pixel]",
    "Width of the detection group"
  );
  plugin_api.getOutputRegistry().registerColumnConverter<DetectionFrameGroupStamp, int>(
    "group_stamp_height",
    [](const DetectionFrameGroupStamp &d) {
      return d.getStamp().getHeight();
    },
    "[pixel]",
    "Height of the detection group"
  );
  plugin_api.getOutputRegistry().enableOutput<DetectionFrameGroupStamp>("GroupStamp");
}

std::string DetectionFrameGroupStampPlugin::getIdString() const {
  return "DetectionFrameGroupStamp";
}

}



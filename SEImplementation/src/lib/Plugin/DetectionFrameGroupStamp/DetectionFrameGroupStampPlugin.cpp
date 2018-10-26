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
    }
  );
  plugin_api.getOutputRegistry().registerColumnConverter<DetectionFrameGroupStamp, int>(
    "group_stamp_left",
    [](const DetectionFrameGroupStamp &d) {
      return d.getTopLeft().m_x;
    }
  );
  plugin_api.getOutputRegistry().registerColumnConverter<DetectionFrameGroupStamp, int>(
    "group_stamp_width",
    [](const DetectionFrameGroupStamp &d) {
      return d.getStamp().getWidth();
    }
  );
  plugin_api.getOutputRegistry().registerColumnConverter<DetectionFrameGroupStamp, int>(
    "group_stamp_height",
    [](const DetectionFrameGroupStamp &d) {
      return d.getStamp().getHeight();
    }
  );
  plugin_api.getOutputRegistry().optionalOutput<DetectionFrameGroupStamp>("GroupStamp");
}

std::string DetectionFrameGroupStampPlugin::getIdString() const {
  return "DetectionFrameGroupStamp";
}

}



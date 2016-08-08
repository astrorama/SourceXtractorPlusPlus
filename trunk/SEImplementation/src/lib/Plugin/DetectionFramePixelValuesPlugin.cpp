/*
 * DetectionFramePixelValuesPlugin.cpp
 *
 *  Created on: Aug 5, 2016
 *      Author: mschefer
 */

#include "SEFramework/Plugin/StaticPlugin.h"

#include "SEImplementation/Property/DetectionFramePixelValues.h"
#include "SEImplementation/Task/DetectionFramePixelValuesTaskFactory.h"

#include "SEImplementation/Plugin/DetectionFramePixelValuesPlugin.h"

namespace SExtractor {

static StaticPlugin<DetectionFramePixelValuesPlugin> detection_frame_pixel_values_plugin;

void DetectionFramePixelValuesPlugin::registerPlugin(PluginAPI& plugin_api) {
  plugin_api.getTaskFactoryRegistry().registerTaskFactory<DetectionFramePixelValues>(
      std::unique_ptr<TaskFactory>(new DetectionFramePixelValuesTaskFactory));
}

std::string DetectionFramePixelValuesPlugin::getIdString() const {
  return "";
}

}


/*
 * DetectionFramePixelValuesPlugin.cpp
 *
 *  Created on: Aug 5, 2016
 *      Author: mschefer
 */

#include "SEFramework/Plugin/StaticPlugin.h"

#include "SEImplementation/Plugin/DetectionFramePixelValues/DetectionFramePixelValues.h"
#include "SEImplementation/Plugin/DetectionFramePixelValues/DetectionFramePixelValuesTaskFactory.h"

#include "SEImplementation/Plugin/DetectionFramePixelValues/DetectionFramePixelValuesPlugin.h"

namespace SExtractor {

static StaticPlugin<DetectionFramePixelValuesPlugin> detection_frame_pixel_values_plugin;

void DetectionFramePixelValuesPlugin::registerPlugin(PluginAPI& plugin_api) {
  plugin_api.getTaskFactoryRegistry().registerTaskFactory<DetectionFramePixelValuesTaskFactory, DetectionFramePixelValues>();
}

std::string DetectionFramePixelValuesPlugin::getIdString() const {
  return "";
}

}


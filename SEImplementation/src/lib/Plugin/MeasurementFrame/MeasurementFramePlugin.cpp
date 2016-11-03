/*
 * MeasurementFramePlugin.cpp
 *
 *  Created on: Nov 3, 2016
 *      Author: mschefer
 */


#include "SEFramework/Plugin/StaticPlugin.h"

#include "SEImplementation/Plugin/MeasurementFrame/MeasurementFrame.h"
#include "SEImplementation/Plugin/MeasurementFrame/MeasurementFrameTaskFactory.h"

#include "SEImplementation/Plugin/MeasurementFrame/MeasurementFramePlugin.h"

namespace SExtractor {

static StaticPlugin<MeasurementFramePlugin> measurement_frame_pixel_centroid_plugin;

void MeasurementFramePlugin::registerPlugin(PluginAPI& plugin_api) {
  plugin_api.getTaskFactoryRegistry()
      .registerTaskFactory<MeasurementFrameTaskFactory, MeasurementFrame>();
}

std::string MeasurementFramePlugin::getIdString() const {
  return "MeasurementFramePlugin";
}

}




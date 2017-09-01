/*
 * MeasurementFramePixelCentroidPlugin.cpp
 *
 *  Created on: Oct 31, 2016
 *      Author: mschefer
 */

#include "SEFramework/Plugin/StaticPlugin.h"

#include "SEImplementation/Plugin/MeasurementFramePixelCentroid/MeasurementFramePixelCentroid.h"
#include "SEImplementation/Plugin/MeasurementFramePixelCentroid/MeasurementFramePixelCentroidTaskFactory.h"

#include "SEImplementation/Plugin/MeasurementFramePixelCentroid/MeasurementFramePixelCentroidPlugin.h"

namespace SExtractor {

static StaticPlugin<MeasurementFramePixelCentroidPlugin> measurement_frame_pixel_centroid_plugin;

void MeasurementFramePixelCentroidPlugin::registerPlugin(PluginAPI& plugin_api) {
  plugin_api.getTaskFactoryRegistry()
      .registerTaskFactory<MeasurementFramePixelCentroidTaskFactory, MeasurementFramePixelCentroid>();
}

std::string MeasurementFramePixelCentroidPlugin::getIdString() const {
  return "MeasurementFramePixelCentroidPlugin";
}

}




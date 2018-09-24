/*
 * MeasurementFrameRectanglePlugin.cpp
 *
 *  Created on: Sep 24, 2018
 *      Author: Alejandro Alvarez Ayllon
 */

#include "SEFramework/Plugin/StaticPlugin.h"

#include "SEImplementation/Plugin/MeasurementFrameRectangle/MeasurementFrameRectangle.h"
#include "SEImplementation/Plugin/MeasurementFrameRectangle/MeasurementFrameRectangleTaskFactory.h"
#include "SEImplementation/Plugin/MeasurementFrameRectangle/MeasurementFrameRectanglePlugin.h"


namespace SExtractor {

static StaticPlugin<MeasurementFrameRectanglePlugin> measurement_frame_rectangle_plugin;

void MeasurementFrameRectanglePlugin::registerPlugin(PluginAPI &plugin_api) {
  plugin_api.getTaskFactoryRegistry().registerTaskFactory<MeasurementFrameRectangleTaskFactory, MeasurementFrameRectangle>();
}

std::string MeasurementFrameRectanglePlugin::getIdString() const {
  return "MeasurementFrameRectangle";
}

}

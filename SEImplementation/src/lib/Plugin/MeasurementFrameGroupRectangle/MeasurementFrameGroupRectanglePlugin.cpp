/*
 * MeasurementFrameGroupRectanglePlugin.cpp
 *
 *  Created on: Sep 5, 2018
 *      Author: Alejandro Alvarez Ayllon
 */

#include "SEFramework/Plugin/StaticPlugin.h"

#include "SEImplementation/Plugin/MeasurementFrameGroupRectangle/MeasurementFrameGroupRectangle.h"
#include "SEImplementation/Plugin/MeasurementFrameGroupRectangle/MeasurementFrameGroupRectangleTaskFactory.h"
#include "SEImplementation/Plugin/MeasurementFrameGroupRectangle/MeasurementFrameGroupRectanglePlugin.h"


namespace SExtractor {

static StaticPlugin<MeasurementFrameGroupRectanglePlugin> measurement_frame_group_rectangle_plugin;

void MeasurementFrameGroupRectanglePlugin::registerPlugin(PluginAPI &plugin_api) {
  plugin_api.getTaskFactoryRegistry().registerTaskFactory<MeasurementFrameGroupRectangleTaskFactory, MeasurementFrameGroupRectangle>();
}

std::string MeasurementFrameGroupRectanglePlugin::getIdString() const {
  return "MeasurementFrameGroupRectangle";
}

}

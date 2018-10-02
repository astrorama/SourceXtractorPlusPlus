/*
 * MeasurementNeighbourInfoPlugin.cpp
 *
 *  Created on: Sep 28, 2018
 *      Author: Alejandro Alvarez Ayllon
 */

#include "SEFramework/Plugin/StaticPlugin.h"
#include "SEImplementation/Plugin/MeasurementNeighbourInfo/MeasurementNeighbourInfoPlugin.h"
#include "SEImplementation/Plugin/MeasurementNeighbourInfo/MeasurementNeighbourInfoTaskFactory.h"
#include "SEImplementation/Plugin/MeasurementNeighbourInfo/MeasurementNeighbourInfo.h"


namespace SExtractor {

static StaticPlugin<MeasurementNeighbourInfoPlugin> neighbour_info_plugin;

void MeasurementNeighbourInfoPlugin::registerPlugin(PluginAPI& plugin_api) {
  plugin_api.getTaskFactoryRegistry().registerTaskFactory<MeasurementNeighbourInfoTaskFactory, MeasurementNeighbourInfo>();
}

std::string MeasurementNeighbourInfoPlugin::getIdString() const {
  return "MeasurementNeighbourInfoPlugin";
}

} // end SExtractor

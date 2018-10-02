/*
 * DetectionNeighbourInfoPlugin.cpp
 *
 *  Created on: Oct 02, 2018
 *      Author: Alejandro Alvarez Ayllon
 */

#include "SEFramework/Plugin/StaticPlugin.h"
#include "SEImplementation/Plugin/DetectionNeighbourInfo/DetectionNeighbourInfoPlugin.h"
#include "SEImplementation/Plugin/DetectionNeighbourInfo/DetectionNeighbourInfoTaskFactory.h"
#include "SEImplementation/Plugin/DetectionNeighbourInfo/DetectionNeighbourInfo.h"


namespace SExtractor {

static StaticPlugin<DetectionNeighbourInfoPlugin> neighbour_info_plugin;

void DetectionNeighbourInfoPlugin::registerPlugin(PluginAPI& plugin_api) {
  plugin_api.getTaskFactoryRegistry().registerTaskFactory<DetectionNeighbourInfoTaskFactory, DetectionNeighbourInfo>();
}

std::string DetectionNeighbourInfoPlugin::getIdString() const {
  return "DetectionNeighbourInfoPlugin";
}

} // end SExtractor

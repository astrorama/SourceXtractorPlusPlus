/*
 * NeighbourInfoPlugin.cpp
 *
 *  Created on: Oct 02, 2018
 *      Author: Alejandro Alvarez Ayllon
 */

#include "SEFramework/Plugin/StaticPlugin.h"
#include "SEImplementation/Plugin/NeighbourInfo/NeighbourInfoPlugin.h"
#include "SEImplementation/Plugin/NeighbourInfo/NeighbourInfoTaskFactory.h"
#include "SEImplementation/Plugin/NeighbourInfo/NeighbourInfo.h"


namespace SExtractor {

static StaticPlugin<NeighbourInfoPlugin> neighbour_info_plugin;

void NeighbourInfoPlugin::registerPlugin(PluginAPI& plugin_api) {
  plugin_api.getTaskFactoryRegistry().registerTaskFactory<NeighbourInfoTaskFactory, NeighbourInfo>();
}

std::string NeighbourInfoPlugin::getIdString() const {
  return "NeighbourInfoPlugin";
}

} // end SExtractor

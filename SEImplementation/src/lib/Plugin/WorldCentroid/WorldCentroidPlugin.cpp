/*
 * WorldCentroidPlugin.cpp
 *
 *  Created on: Nov 21, 2016
 *      Author: mschefer
 */


#include "SEFramework/Plugin/StaticPlugin.h"

#include "SEImplementation/Plugin/WorldCentroid/WorldCentroid.h"
#include "SEImplementation/Plugin/WorldCentroid/WorldCentroidTaskFactory.h"

#include "SEImplementation/Plugin/WorldCentroid/WorldCentroidPlugin.h"

namespace SExtractor {

static StaticPlugin<WorldCentroidPlugin> pixel_centroid_plugin;

void WorldCentroidPlugin::registerPlugin(PluginAPI& plugin_api) {
  plugin_api.getTaskFactoryRegistry().registerTaskFactory<WorldCentroidTaskFactory, WorldCentroid>();

  plugin_api.getOutputRegistry().registerColumnConverter<WorldCentroid, double>(
          "world_centroid_alpha",
          [](const WorldCentroid& prop){
            return prop.getCentroidAlpha();
          }
  );

  plugin_api.getOutputRegistry().registerColumnConverter<WorldCentroid, double>(
          "world_centroid_delta",
          [](const WorldCentroid& prop){
            return prop.getCentroidDelta();
          }
  );

  plugin_api.getOutputRegistry().enableOutput<WorldCentroid>("WorldCentroid");
}

std::string WorldCentroidPlugin::getIdString() const {
  return "World Centroid";
}

}






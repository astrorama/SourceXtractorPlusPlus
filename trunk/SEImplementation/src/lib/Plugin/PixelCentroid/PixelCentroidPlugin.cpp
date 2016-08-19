/*
 * PixelCentroidPlugin.cpp
 *
 *  Created on: Aug 5, 2016
 *      Author: mschefer
 */

#include "SEFramework/Plugin/StaticPlugin.h"

#include "SEImplementation/Plugin/PixelCentroid/PixelCentroid.h"
#include "SEImplementation/Plugin/PixelCentroid/PixelCentroidTaskFactory.h"

#include "SEImplementation/Plugin/PixelCentroid/PixelCentroidPlugin.h"

namespace SExtractor {

static StaticPlugin<PixelCentroidPlugin> pixel_centroid_plugin;

void PixelCentroidPlugin::registerPlugin(PluginAPI& plugin_api) {
  plugin_api.getTaskFactoryRegistry().registerTaskFactory<PixelCentroidTaskFactory, PixelCentroid>();

  plugin_api.getOutputRegistry().registerColumnConverter<PixelCentroid, double>(
          "pixel_centroid_x",
          [](const PixelCentroid& prop){
            return prop.getCentroidX();
          }
  );

  plugin_api.getOutputRegistry().registerColumnConverter<PixelCentroid, double>(
          "pixel_centroid_y",
          [](const PixelCentroid& prop){
            return prop.getCentroidY();
          }
  );

  plugin_api.getOutputRegistry().optionalOutput<PixelCentroid>("PixelCentroid");
}

std::string PixelCentroidPlugin::getIdString() const {
  return "";
}

}



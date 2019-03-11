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
            return prop.getCentroidX() + 1.0; // add one to use FITS standard coordinates
          },
          "[pixel]",
          "X-position of the object in the detection image"
  );

  plugin_api.getOutputRegistry().registerColumnConverter<PixelCentroid, double>(
          "pixel_centroid_y",
          [](const PixelCentroid& prop){
            return prop.getCentroidY() + 1.0; // add one to use FITS standard coordinates
          },
          "[pixel]",
          "Y-position of the object in the detection image"
  );

  plugin_api.getOutputRegistry().enableOutput<PixelCentroid>("PixelCentroid");
}

std::string PixelCentroidPlugin::getIdString() const {
  return "PixelCentroid";
}

}



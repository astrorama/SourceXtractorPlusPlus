/*
 * PixelBoundariesPlugin.cpp
 *
 *  Created on: Aug 5, 2016
 *      Author: mschefer
 */


#include "SEFramework/Plugin/StaticPlugin.h"

#include "SEImplementation/Plugin/PixelBoundaries/PixelBoundaries.h"
#include "SEImplementation/Plugin/PixelBoundaries/PixelBoundariesTaskFactory.h"

#include "SEImplementation/Plugin/PixelBoundaries/PixelBoundariesPlugin.h"

namespace SExtractor {

static StaticPlugin<PixelBoundariesPlugin> pixel_boundaries_plugin;

void PixelBoundariesPlugin::registerPlugin(PluginAPI& plugin_api) {
  plugin_api.getTaskFactoryRegistry().registerTaskFactory<PixelBoundariesTaskFactory, PixelBoundaries>();
  plugin_api.getTaskFactoryRegistry().registerTaskFactory<PixelBoundariesTaskFactory, PixelBoundariesHalfMaximum>();

  plugin_api.getOutputRegistry().registerColumnConverter<PixelBoundaries, int>(
          "pixel_boundaries_min_x",
          [](const PixelBoundaries& prop){
            return prop.getMin().m_x;
          },
          "[pixel]",
          "Minimum x-coordinate of the detection area"
  );

  plugin_api.getOutputRegistry().registerColumnConverter<PixelBoundaries, int>(
          "pixel_boundaries_min_y",
          [](const PixelBoundaries& prop){
            return prop.getMin().m_y;
          },
          "[pixel]",
          "Minimum y-coordinate of the detection area"
  );

  plugin_api.getOutputRegistry().registerColumnConverter<PixelBoundaries, int>(
          "pixel_boundaries_max_x",
          [](const PixelBoundaries& prop){
            return prop.getMax().m_x;
          },
          "[pixel]",
          "Maximum x-coordinate of the detection area"
  );

  plugin_api.getOutputRegistry().registerColumnConverter<PixelBoundaries, int>(
          "pixel_boundaries_max_y",
          [](const PixelBoundaries& prop){
            return prop.getMax().m_y;
          },
          "[pixel]",
          "Maximum y-coordinate of the detection area"
  );

  plugin_api.getOutputRegistry().enableOutput<PixelBoundaries>("PixelBoundaries");

}

std::string PixelBoundariesPlugin::getIdString() const {
  return "";
}

}



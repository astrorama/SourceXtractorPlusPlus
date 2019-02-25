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
          }
  );

  plugin_api.getOutputRegistry().registerColumnConverter<PixelBoundaries, int>(
          "pixel_boundaries_min_y",
          [](const PixelBoundaries& prop){
            return prop.getMin().m_y;
          }
  );

  plugin_api.getOutputRegistry().registerColumnConverter<PixelBoundaries, int>(
          "pixel_boundaries_max_x",
          [](const PixelBoundaries& prop){
            return prop.getMax().m_x;
          }
  );

  plugin_api.getOutputRegistry().registerColumnConverter<PixelBoundaries, int>(
          "pixel_boundaries_max_y",
          [](const PixelBoundaries& prop){
            return prop.getMax().m_y;
          }
  );

  plugin_api.getOutputRegistry().enableOutput<PixelBoundaries>("PixelBoundaries");

}

std::string PixelBoundariesPlugin::getIdString() const {
  return "";
}

}



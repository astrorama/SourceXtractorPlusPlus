/** Copyright © 2019 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3.0 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */
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
          },
          "[deg]",
          "RA object position"
  );

  plugin_api.getOutputRegistry().registerColumnConverter<WorldCentroid, double>(
          "world_centroid_delta",
          [](const WorldCentroid& prop){
            return prop.getCentroidDelta();
          },
          "[deg]",
          "Dec object position"
  );

  plugin_api.getOutputRegistry().enableOutput<WorldCentroid>("WorldCentroid");
}

std::string WorldCentroidPlugin::getIdString() const {
  return "World Centroid";
}

}






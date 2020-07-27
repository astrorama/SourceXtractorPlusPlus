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
 * PixelBoundariesPlugin.cpp
 *
 *  Created on: Aug 5, 2016
 *      Author: mschefer
 */


#include "SEFramework/Plugin/StaticPlugin.h"

#include "SEImplementation/Plugin/PixelBoundaries/PixelBoundaries.h"
#include "SEImplementation/Plugin/PixelBoundaries/PixelBoundariesTaskFactory.h"

#include "SEImplementation/Plugin/PixelBoundaries/PixelBoundariesPlugin.h"

namespace SourceXtractor {

static StaticPlugin<PixelBoundariesPlugin> pixel_boundaries_plugin;

void PixelBoundariesPlugin::registerPlugin(PluginAPI& plugin_api) {
  plugin_api.getTaskFactoryRegistry().registerTaskFactory<PixelBoundariesTaskFactory, PixelBoundaries>();
  plugin_api.getTaskFactoryRegistry().registerTaskFactory<PixelBoundariesTaskFactory, PixelBoundariesHalfMaximum>();

  plugin_api.getOutputRegistry().registerColumnConverter<PixelBoundaries, int>(
          "pixel_boundaries_min_x",
          [](const PixelBoundaries& prop){
            return prop.getMin().m_x;
          },
          "pixel",
          "Minimum x-coordinate of the detection area"
  );

  plugin_api.getOutputRegistry().registerColumnConverter<PixelBoundaries, int>(
          "pixel_boundaries_min_y",
          [](const PixelBoundaries& prop){
            return prop.getMin().m_y;
          },
          "pixel",
          "Minimum y-coordinate of the detection area"
  );

  plugin_api.getOutputRegistry().registerColumnConverter<PixelBoundaries, int>(
          "pixel_boundaries_max_x",
          [](const PixelBoundaries& prop){
            return prop.getMax().m_x;
          },
          "pixel",
          "Maximum x-coordinate of the detection area"
  );

  plugin_api.getOutputRegistry().registerColumnConverter<PixelBoundaries, int>(
          "pixel_boundaries_max_y",
          [](const PixelBoundaries& prop){
            return prop.getMax().m_y;
          },
          "pixel",
          "Maximum y-coordinate of the detection area"
  );

  plugin_api.getOutputRegistry().enableOutput<PixelBoundaries>("PixelBoundaries");

}

std::string PixelBoundariesPlugin::getIdString() const {
  return "";
}

}



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
 * PixelCentroidPlugin.cpp
 *
 *  Created on: Aug 5, 2016
 *      Author: mschefer
 */

#include "SEFramework/Plugin/StaticPlugin.h"

#include "SEImplementation/Plugin/PixelCentroid/PixelCentroid.h"
#include "SEImplementation/Plugin/PixelCentroid/PixelCentroidTaskFactory.h"

#include "SEImplementation/Plugin/PixelCentroid/PixelCentroidPlugin.h"

namespace SourceXtractor {

static StaticPlugin<PixelCentroidPlugin> pixel_centroid_plugin;

void PixelCentroidPlugin::registerPlugin(PluginAPI& plugin_api) {
  plugin_api.getTaskFactoryRegistry().registerTaskFactory<PixelCentroidTaskFactory, PixelCentroid>();

  plugin_api.getOutputRegistry().registerColumnConverter<PixelCentroid, double>(
          "pixel_centroid_x",
          [](const PixelCentroid& prop){
            return prop.getCentroidX() + 1.0; // add one to use FITS standard coordinates
          },
          "pixel",
          "X-position of the object in the detection image"
  );

  plugin_api.getOutputRegistry().registerColumnConverter<PixelCentroid, double>(
          "pixel_centroid_y",
          [](const PixelCentroid& prop){
            return prop.getCentroidY() + 1.0; // add one to use FITS standard coordinates
          },
          "pixel",
          "Y-position of the object in the detection image"
  );

  plugin_api.getOutputRegistry().enableOutput<PixelCentroid>("PixelCentroid");
}

std::string PixelCentroidPlugin::getIdString() const {
  return "PixelCentroid";
}

}



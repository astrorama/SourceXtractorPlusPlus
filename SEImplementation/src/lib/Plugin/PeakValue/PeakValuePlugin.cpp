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
 * PeakValuePlugin.cpp
 *
 *  Created on: Feb 9, 2017
 *      Author: mschefer
 */

#include "SEFramework/Plugin/StaticPlugin.h"

#include "SEImplementation/Plugin/PeakValue/PeakValue.h"
#include "SEImplementation/Plugin/PeakValue/PeakValueTaskFactory.h"

#include "SEImplementation/Plugin/PeakValue/PeakValuePlugin.h"

namespace SExtractor {

static StaticPlugin<PeakValuePlugin> peak_value_plugin;

void PeakValuePlugin::registerPlugin(PluginAPI& plugin_api) {
  plugin_api.getTaskFactoryRegistry().registerTaskFactory<PeakValueTaskFactory, PeakValue>();

  plugin_api.getOutputRegistry().registerColumnConverter<PeakValue, double>(
          "peak_value",
          [](const PeakValue& prop){
            return prop.getMaxValue();
          },
          "[count]",
          "Highest pixel value in the detection image"
  );

  plugin_api.getOutputRegistry().enableOutput<PeakValue>("PeakValue");
}

std::string PeakValuePlugin::getIdString() const {
  return "PeakValue";
}

}


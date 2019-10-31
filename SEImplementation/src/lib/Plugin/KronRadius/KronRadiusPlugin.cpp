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
 * KronRadiusPlugin.cpp
 *
 *  Created on: Sep 12, 2016
 *      Author: mkuemmel@usm.lmu.de
 */

#include "SEFramework/Plugin/StaticPlugin.h"

#include "SEImplementation/Plugin/KronRadius/KronRadius.h"
#include "SEImplementation/Plugin/KronRadius/KronRadiusTaskFactory.h"
#include "SEImplementation/Plugin/KronRadius/KronRadiusPlugin.h"

namespace SourceXtractor {

static StaticPlugin<KronRadiusPlugin> kron_radius_plugin;

void KronRadiusPlugin::registerPlugin(PluginAPI& plugin_api) {
  plugin_api.getTaskFactoryRegistry().registerTaskFactory<KronRadiusTaskFactory, KronRadius>();

  plugin_api.getOutputRegistry().registerColumnConverter<KronRadius, double>(
          "kron_radius",
          [](const KronRadius& prop){
            return prop.getKronRadius();
          },
          "[pixel]",
          "Kron radius"
  );

  plugin_api.getOutputRegistry().registerColumnConverter<KronRadius, int64_t>(
          "kron_flag",
          [](const KronRadius& prop){
            return prop.getFlag();
          },
          "[]",
          "Flags for the Kron radius"
  );

  // register as optional output (to have it in the output catalog)
  plugin_api.getOutputRegistry().enableOutput<KronRadius>("KronRadius");
}

std::string KronRadiusPlugin::getIdString() const {
  return "KronRadiusPlugin";
}

}

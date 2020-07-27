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
 * IsophotalFluxPlugin.cpp
 *
 *  Created on: Sep 28, 2016
 *      Author: mschefer
 */

#include "SEFramework/Plugin/StaticPlugin.h"

#include "SEImplementation/Plugin/IsophotalFlux/IsophotalFlux.h"
#include "SEImplementation/Plugin/IsophotalFlux/IsophotalFluxTaskFactory.h"

#include "SEImplementation/Plugin/IsophotalFlux/IsophotalFluxPlugin.h"

namespace SourceXtractor {

static StaticPlugin<IsophotalFluxPlugin> isophotal_flux_plugin;

void IsophotalFluxPlugin::registerPlugin(PluginAPI& plugin_api) {
  plugin_api.getTaskFactoryRegistry().registerTaskFactory<IsophotalFluxTaskFactory, IsophotalFlux>();

  plugin_api.getOutputRegistry().registerColumnConverter<IsophotalFlux, double>(
          "isophotal_flux",
          [](const IsophotalFlux& prop){
            return prop.getFlux();
          },
          "count",
          "Isophotal flux"
  );

  plugin_api.getOutputRegistry().registerColumnConverter<IsophotalFlux, double>(
          "isophotal_flux_err",
          [](const IsophotalFlux& prop){
            return prop.getFluxError();
          },
          "count",
          "Isophotal flux error"
  );

  plugin_api.getOutputRegistry().registerColumnConverter<IsophotalFlux, double>(
          "isophotal_mag",
          [](const IsophotalFlux& prop){
            return prop.getMag();
          },
          "mag",
          "Isophotal magnitude"
  );

  plugin_api.getOutputRegistry().registerColumnConverter<IsophotalFlux, double>(
          "isophotal_mag_err",
          [](const IsophotalFlux& prop){
            return prop.getMagError();
          },
          "mag",
          "Isophotal magnitude error"
  );



  plugin_api.getOutputRegistry().enableOutput<IsophotalFlux>("IsophotalFlux");
}

std::string IsophotalFluxPlugin::getIdString() const {
  return "IsophotalFlux";
}

}


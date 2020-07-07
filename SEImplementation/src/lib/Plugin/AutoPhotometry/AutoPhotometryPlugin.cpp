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
 * AutoPhotometryPlugin.cpp
 *
 *  Created on: Jul 18, 2016
 *      Author: mkuemmel@usm.lmu.de
 */

#include "SEFramework/Plugin/StaticPlugin.h"

#include "SEImplementation/Plugin/AutoPhotometry/AutoPhotometry.h"
#include "SEImplementation/Plugin/AutoPhotometry/AutoPhotometryFlag.h"
#include "SEImplementation/Plugin/AutoPhotometry/AutoPhotometryArray.h"
#include "SEImplementation/Plugin/AutoPhotometry/AutoPhotometryTaskFactory.h"
#include "SEImplementation/Plugin/AutoPhotometry/AutoPhotometryPlugin.h"

namespace SourceXtractor {

static StaticPlugin<AutoPhotometryPlugin> aperture_photometry_plugin;

void AutoPhotometryPlugin::registerPlugin(PluginAPI& plugin_api) {
  plugin_api.getTaskFactoryRegistry()
    .registerTaskFactory<AutoPhotometryTaskFactory, AutoPhotometry, AutoPhotometryFlag, AutoPhotometryArray>();

  plugin_api.getOutputRegistry().registerColumnConverter<AutoPhotometryArray, std::vector<SeFloat>>(
          "auto_flux",
          [](const AutoPhotometryArray& prop){
            return prop.getFluxes();
          },
          "count",
          "Flux within a Kron-like elliptical aperture"
          );

  plugin_api.getOutputRegistry().registerColumnConverter<AutoPhotometryArray, std::vector<SeFloat>>(
          "auto_flux_err",
          [](const AutoPhotometryArray& prop){
            return prop.getFluxErrors();
          },
          "count",
          "Flux error within a Kron-like elliptical aperture"
          );

  plugin_api.getOutputRegistry().registerColumnConverter<AutoPhotometryArray, std::vector<SeFloat>>(
          "auto_mag",
          [](const AutoPhotometryArray& prop){
            return prop.getMags();
          },
          "mag",
          "Magnitude within a Kron-like elliptical aperture"
          );

  plugin_api.getOutputRegistry().registerColumnConverter<AutoPhotometryArray, std::vector<SeFloat>>(
          "auto_mag_err",
          [](const AutoPhotometryArray& prop){
            return prop.getMagErrors();
          },
          "mag",
          "Magnitude error within a Kron-like elliptical aperture"
          );

  plugin_api.getOutputRegistry().registerColumnConverter<AutoPhotometryArray, std::vector<int64_t >>(
          "auto_flags",
          [](const AutoPhotometryArray& prop){
            return flags2long(prop.getFlags());
          },
          "",
          "Flags for the Kron-like elliptical aperture photometry"
          );

  plugin_api.getOutputRegistry().enableOutput<AutoPhotometryArray>("AutoPhotometry");
}

std::string AutoPhotometryPlugin::getIdString() const {
  return "AutoPhotometryPlugin";
}

}

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
 * AperturePhotometryPlugin.cpp
 *
 *  Created on: Sep 23, 2016
 *      Author: mschefer
 */

#include "SEFramework/Plugin/StaticPlugin.h"

#include "SEImplementation/Plugin/AperturePhotometry/AperturePhotometry.h"
#include "SEImplementation/Plugin/AperturePhotometry/ApertureFlag.h"
#include "SEImplementation/Plugin/AperturePhotometry/AperturePhotometryArray.h"
#include "SEImplementation/Plugin/AperturePhotometry/AperturePhotometryTaskFactory.h"
#include "SEImplementation/Plugin/AperturePhotometry/AperturePhotometryPlugin.h"

namespace SourceXtractor {

static StaticPlugin<AperturePhotometryPlugin> aperture_photometry_plugin;

template <typename T>
using NdArray = Euclid::NdArray::NdArray<T>;

void AperturePhotometryPlugin::registerPlugin(PluginAPI &plugin_api) {
  plugin_api.getTaskFactoryRegistry()
  .registerTaskFactory<AperturePhotometryTaskFactory, AperturePhotometry, ApertureFlag, AperturePhotometryArray>();

  plugin_api.getOutputRegistry().registerColumnConverter<AperturePhotometryArray, NdArray<SeFloat>>(
    "aperture_flux",
    [](const AperturePhotometryArray &prop) {
      return prop.getFluxes();
    },
    "[count]",
    "Aperture flux"
  );

  plugin_api.getOutputRegistry().registerColumnConverter<AperturePhotometryArray, NdArray<SeFloat>>(
    "aperture_flux_err",
    [](const AperturePhotometryArray &prop) {
      return prop.getFluxErrors();
    },
    "[count]",
    "Aperture flux error"
  );

  plugin_api.getOutputRegistry().registerColumnConverter<AperturePhotometryArray, NdArray<SeFloat>>(
    "aperture_mag",
    [](const AperturePhotometryArray &prop) {
      return prop.getMags();
    },
    "[mag]",
    "Aperture magnitude"
  );

  plugin_api.getOutputRegistry().registerColumnConverter<AperturePhotometryArray, NdArray<SeFloat>>(
    "aperture_mag_err",
    [](const AperturePhotometryArray &prop) {
      return prop.getMagErrors();
    },
    "[mag]",
    "Aperture magnitude error"
  );

  plugin_api.getOutputRegistry().registerColumnConverter<AperturePhotometryArray, NdArray<int64_t>>(
    "aperture_flags",
    [](const AperturePhotometryArray &prop) {
      return prop.getFlags();
    },
    "",
    "Aperture flags"
  );

  plugin_api.getOutputRegistry().enableOutput<AperturePhotometryArray>("AperturePhotometry");
}

std::string AperturePhotometryPlugin::getIdString() const {
  return "AperturePhotometryPlugin";
}

}

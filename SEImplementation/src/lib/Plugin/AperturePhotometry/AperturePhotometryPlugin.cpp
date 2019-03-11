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

namespace SExtractor {

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

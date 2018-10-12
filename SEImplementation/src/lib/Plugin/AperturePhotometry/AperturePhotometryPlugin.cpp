/*
 * AperturePhotometryPlugin.cpp
 *
 *  Created on: Sep 23, 2016
 *      Author: mschefer
 */

#include "SEFramework/Plugin/StaticPlugin.h"

#include "SEImplementation/Plugin/AperturePhotometry/AperturePhotometry.h"
#include "SEImplementation/Plugin/AperturePhotometry/ApertureFlag.h"
#include "SEImplementation/Plugin/AperturePhotometry/AperturePhotometryTaskFactory.h"
#include "SEImplementation/Plugin/AperturePhotometry/AperturePhotometryPlugin.h"

namespace SExtractor {

static StaticPlugin<AperturePhotometryPlugin> aperture_photometry_plugin;

void AperturePhotometryPlugin::registerPlugin(PluginAPI &plugin_api) {
  plugin_api.getTaskFactoryRegistry().registerTaskFactory<AperturePhotometryTaskFactory, AperturePhotometry, ApertureFlag>();

  plugin_api.getOutputRegistry().registerColumnConverter<AperturePhotometry, double>(
    "aperture_flux",
    [](const AperturePhotometry &prop) {
      return prop.getFlux();
    }
  );

  plugin_api.getOutputRegistry().registerColumnConverter<AperturePhotometry, double>(
    "aperture_flux_err",
    [](const AperturePhotometry &prop) {
      return prop.getFluxError();
    }
  );

  plugin_api.getOutputRegistry().registerColumnConverter<AperturePhotometry, double>(
    "aperture_mag",
    [](const AperturePhotometry &prop) {
      return prop.getMag();
    }
  );

  plugin_api.getOutputRegistry().registerColumnConverter<AperturePhotometry, double>(
    "aperture_mag_err",
    [](const AperturePhotometry &prop) {
      return prop.getMagError();
    }
  );

  plugin_api.getOutputRegistry().registerColumnConverter<ApertureFlag, std::vector<long>>(
    "aperture_flags",
    [](const ApertureFlag &prop) {
      return prop.getFlags();
    }
  );

  // register as optional output (to have it in the output catalog)
  plugin_api.getOutputRegistry().optionalOutput<AperturePhotometry>("AperturePhotometry");
  plugin_api.getOutputRegistry().optionalOutput<ApertureFlag>("AperturePhotometry");
}

std::string AperturePhotometryPlugin::getIdString() const {
  return "AperturePhotometryPlugin";
}

}

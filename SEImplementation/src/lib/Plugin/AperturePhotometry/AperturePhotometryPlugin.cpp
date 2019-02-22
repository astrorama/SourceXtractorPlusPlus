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

  plugin_api.getOutputRegistry().registerColumnConverter<AperturePhotometry, std::vector<SeFloat>>(
    "aperture_flux",
    [](const AperturePhotometry &prop) {
      return prop.getFluxes();
    }
  );

  plugin_api.getOutputRegistry().registerColumnConverter<AperturePhotometry, std::vector<SeFloat>>(
    "aperture_flux_err",
    [](const AperturePhotometry &prop) {
      return prop.getFluxErrors();
    }
  );

  plugin_api.getOutputRegistry().registerColumnConverter<AperturePhotometry, std::vector<SeFloat>>(
    "aperture_mag",
    [](const AperturePhotometry &prop) {
      return prop.getMags();
    }
  );

  plugin_api.getOutputRegistry().registerColumnConverter<AperturePhotometry, std::vector<SeFloat>>(
    "aperture_mag_err",
    [](const AperturePhotometry &prop) {
      return prop.getMagErrors();
    }
  );

  plugin_api.getOutputRegistry().registerColumnConverter<AperturePhotometry, std::vector<int64_t>>(
    "aperture_flags",
    [](const AperturePhotometry &prop) {
      return flags2long(prop.getFlags());
    }
  );

  // register as optional output (to have it in the output catalog)
  plugin_api.getOutputRegistry().enableOutput<AperturePhotometry>("AperturePhotometry");
}

std::string AperturePhotometryPlugin::getIdString() const {
  return "AperturePhotometryPlugin";
}

}

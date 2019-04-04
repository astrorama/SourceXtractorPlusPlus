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

namespace SExtractor {

static StaticPlugin<AutoPhotometryPlugin> aperture_photometry_plugin;

void AutoPhotometryPlugin::registerPlugin(PluginAPI& plugin_api) {
  plugin_api.getTaskFactoryRegistry()
    .registerTaskFactory<AutoPhotometryTaskFactory, AutoPhotometry, AutoPhotometryFlag, AutoPhotometryArray>();

  plugin_api.getOutputRegistry().registerColumnConverter<AutoPhotometryArray, std::vector<SeFloat>>(
          "auto_flux",
          [](const AutoPhotometryArray& prop){
            return prop.getFluxes();
          },
          "[count]",
          "Flux within a Kron-like elliptical aperture"
          );

  plugin_api.getOutputRegistry().registerColumnConverter<AutoPhotometryArray, std::vector<SeFloat>>(
          "auto_flux_err",
          [](const AutoPhotometryArray& prop){
            return prop.getFluxErrors();
          },
          "[count]",
          "Flux error within a Kron-like elliptical aperture"
          );

  plugin_api.getOutputRegistry().registerColumnConverter<AutoPhotometryArray, std::vector<SeFloat>>(
          "auto_mag",
          [](const AutoPhotometryArray& prop){
            return prop.getMags();
          },
          "[count]",
          "Magnitude within a Kron-like elliptical aperture"
          );

  plugin_api.getOutputRegistry().registerColumnConverter<AutoPhotometryArray, std::vector<SeFloat>>(
          "auto_mag_err",
          [](const AutoPhotometryArray& prop){
            return prop.getMagErrors();
          },
          "[count]",
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

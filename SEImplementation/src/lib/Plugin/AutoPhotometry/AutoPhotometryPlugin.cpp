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
          }
  );

  plugin_api.getOutputRegistry().registerColumnConverter<AutoPhotometryArray, std::vector<SeFloat>>(
          "auto_flux_err",
          [](const AutoPhotometryArray& prop){
            return prop.getFluxErrors();
          }
  );

  plugin_api.getOutputRegistry().registerColumnConverter<AutoPhotometryArray, std::vector<SeFloat>>(
          "auto_mag",
          [](const AutoPhotometryArray& prop){
            return prop.getMags();
          }
  );

  plugin_api.getOutputRegistry().registerColumnConverter<AutoPhotometryArray, std::vector<SeFloat>>(
          "auto_mag_err",
          [](const AutoPhotometryArray& prop){
            return prop.getMagErrors();
          }
  );

  plugin_api.getOutputRegistry().registerColumnConverter<AutoPhotometryArray, std::vector<int64_t >>(
          "auto_flags",
          [](const AutoPhotometryArray& prop){
            return flags2long(prop.getFlags());
          }
  );

  plugin_api.getOutputRegistry().enableOutput<AutoPhotometryArray>("AutoPhotometry");
}

std::string AutoPhotometryPlugin::getIdString() const {
  return "AutoPhotometryPlugin";
}

}

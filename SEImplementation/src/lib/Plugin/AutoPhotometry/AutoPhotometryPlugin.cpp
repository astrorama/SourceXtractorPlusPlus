/*
 * AutoPhotometryPlugin.cpp
 *
 *  Created on: Jul 18, 2016
 *      Author: mkuemmel@usm.lmu.de
 */

#include "SEFramework/Plugin/StaticPlugin.h"

#include "SEImplementation/Plugin/AutoPhotometry/AutoPhotometry.h"
#include "SEImplementation/Plugin/AutoPhotometry/AutoPhotometryFlag.h"
#include "SEImplementation/Plugin/AutoPhotometry/AutoPhotometryTaskFactory.h"
#include "SEImplementation/Plugin/AutoPhotometry/AutoPhotometryPlugin.h"

namespace SExtractor {

static StaticPlugin<AutoPhotometryPlugin> aperture_photometry_plugin;

void AutoPhotometryPlugin::registerPlugin(PluginAPI& plugin_api) {
  plugin_api.getTaskFactoryRegistry().registerTaskFactory<AutoPhotometryTaskFactory, AutoPhotometry, AutoPhotometryFlag>();

  plugin_api.getOutputRegistry().registerColumnConverter<AutoPhotometry, double>(
          "auto_flux",
          [](const AutoPhotometry& prop){
            return prop.getFlux();
          }
  );

  plugin_api.getOutputRegistry().registerColumnConverter<AutoPhotometry, double>(
          "auto_flux_err",
          [](const AutoPhotometry& prop){
            return prop.getFluxError();
          }
  );

  plugin_api.getOutputRegistry().registerColumnConverter<AutoPhotometry, double>(
          "auto_mag",
          [](const AutoPhotometry& prop){
            return prop.getMag();
          }
  );

  plugin_api.getOutputRegistry().registerColumnConverter<AutoPhotometry, double>(
          "auto_mag_err",
          [](const AutoPhotometry& prop){
            return prop.getMagError();
          }
  );

  plugin_api.getOutputRegistry().registerColumnConverter<AutoPhotometryFlag, long int>(
          "auto_flag",
          [](const AutoPhotometryFlag& prop){
            return prop.getFlag();
          }
  );

  // register as optional output (to have it in the output catalog)
  plugin_api.getOutputRegistry().optionalOutput<AutoPhotometry>("AutoPhotometry");
  plugin_api.getOutputRegistry().optionalOutput<AutoPhotometryFlag>("AutoPhotometry");
}

std::string AutoPhotometryPlugin::getIdString() const {
  return "AutoPhotometryPlugin";
}

}

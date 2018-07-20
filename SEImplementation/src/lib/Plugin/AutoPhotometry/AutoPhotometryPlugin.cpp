/*
 * AutoPhotometryPlugin.cpp
 *
 *  Created on: Jul 18, 2016
 *      Author: mkuemmel@usm.lmu.de
 */

#include "SEFramework/Plugin/StaticPlugin.h"

#include "SEImplementation/Plugin/AutoPhotometry/AutoPhotometry.h"
#include "SEImplementation/Plugin/AutoPhotometry/AutoPhotometryTaskFactory.h"
#include "SEImplementation/Plugin/AutoPhotometry/AutoPhotometryPlugin.h"

namespace SExtractor {

static StaticPlugin<AutoPhotometryPlugin> aperture_photometry_plugin;

void AutoPhotometryPlugin::registerPlugin(PluginAPI& plugin_api) {
  plugin_api.getTaskFactoryRegistry().registerTaskFactory<AutoPhotometryTaskFactory, AutoPhotometry>();

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

  // register as optional output (to have it in the output catalog)
  plugin_api.getOutputRegistry().optionalOutput<AutoPhotometry>("AutoPhotometry");
  // just enable as output (to execute on the measurment images only??)
  //plugin_api.getOutputRegistry().enableOutput<AperturePhotometry>();
}

std::string AutoPhotometryPlugin::getIdString() const {
  return "AutoPhotometryPlugin";
}

}

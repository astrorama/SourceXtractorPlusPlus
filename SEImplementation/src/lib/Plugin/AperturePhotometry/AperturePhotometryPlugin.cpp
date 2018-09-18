/*
 * AperturePhotometryPlugin.cpp
 *
 *  Created on: Sep 23, 2016
 *      Author: mschefer
 */

#include "SEFramework/Plugin/StaticPlugin.h"

#include "SEImplementation/Plugin/AperturePhotometry/AperturePhotometry.h"
#include "SEImplementation/Plugin/AperturePhotometry/AperturePhotometryTaskFactory.h"

#include "SEImplementation/Plugin/AperturePhotometry/AperturePhotometryPlugin.h"

namespace SExtractor {

static StaticPlugin<AperturePhotometryPlugin> aperture_photometry_plugin;

void AperturePhotometryPlugin::registerPlugin(PluginAPI& plugin_api) {
  plugin_api.getTaskFactoryRegistry().registerTaskFactory<AperturePhotometryTaskFactory, AperturePhotometry>();

  plugin_api.getOutputRegistry().registerColumnConverter<AperturePhotometry, double>(
          "aperture_flux",
          [](const AperturePhotometry& prop){
            return prop.getFlux();
          }
  );

  plugin_api.getOutputRegistry().registerColumnConverter<AperturePhotometry, double>(
          "aperture_flux_err",
          [](const AperturePhotometry& prop){
            return prop.getFluxError();
          }
  );

  plugin_api.getOutputRegistry().registerColumnConverter<AperturePhotometry, double>(
          "aperture_mag",
          [](const AperturePhotometry& prop){
            return prop.getMag();
          }
  );

  plugin_api.getOutputRegistry().registerColumnConverter<AperturePhotometry, double>(
          "aperture_mag_err",
          [](const AperturePhotometry& prop){
            return prop.getMagError();
          }
  );

  plugin_api.getOutputRegistry().registerColumnConverter<AperturePhotometry, long int>(
          "aperture_flag",
          [](const AperturePhotometry& prop){
            return prop.getFlag();
          }
  );

  // register as optional output (to have it in the output catalog)
  plugin_api.getOutputRegistry().optionalOutput<AperturePhotometry>("AperturePhotometry");
  // just enable as output (to execute on the measurment images only??)
  //plugin_api.getOutputRegistry().enableOutput<AperturePhotometry>();
}

std::string AperturePhotometryPlugin::getIdString() const {
  return "AperturePhotometryPlugin";
}

}

/*
 * IsophotalFluxPlugin.cpp
 *
 *  Created on: Sep 28, 2016
 *      Author: mschefer
 */

#include "SEFramework/Plugin/StaticPlugin.h"

#include "SEImplementation/Plugin/IsophotalFlux/IsophotalFlux.h"
#include "SEImplementation/Plugin/IsophotalFlux/IsophotalFluxTaskFactory.h"

#include "SEImplementation/Plugin/IsophotalFlux/IsophotalFluxPlugin.h"

namespace SExtractor {

static StaticPlugin<IsophotalFluxPlugin> isophotal_flux_plugin;

void IsophotalFluxPlugin::registerPlugin(PluginAPI& plugin_api) {
  plugin_api.getTaskFactoryRegistry().registerTaskFactory<IsophotalFluxTaskFactory, IsophotalFlux>();

  plugin_api.getOutputRegistry().registerColumnConverter<IsophotalFlux, double>(
          "isophotal_flux",
          [](const IsophotalFlux& prop){
            return prop.getFlux();
          }
  );

  plugin_api.getOutputRegistry().registerColumnConverter<IsophotalFlux, double>(
          "isophotal_flux_err",
          [](const IsophotalFlux& prop){
            return prop.getFluxError();
          }
  );

  plugin_api.getOutputRegistry().registerColumnConverter<IsophotalFlux, double>(
          "isophotal_mag",
          [](const IsophotalFlux& prop){
            return prop.getMag();
          }
  );

  plugin_api.getOutputRegistry().registerColumnConverter<IsophotalFlux, double>(
          "isophotal_mag_err",
          [](const IsophotalFlux& prop){
            return prop.getMagError();
          }
  );



  plugin_api.getOutputRegistry().optionalOutput<IsophotalFlux>("IsophotalFlux");
}

std::string IsophotalFluxPlugin::getIdString() const {
  return "IsophotalFlux";
}

}


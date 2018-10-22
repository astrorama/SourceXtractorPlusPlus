/*
 * KronRadiusPlugin.cpp
 *
 *  Created on: Sep 12, 2016
 *      Author: mkuemmel@usm.lmu.de
 */

#include "SEFramework/Plugin/StaticPlugin.h"

#include "SEImplementation/Plugin/KronRadius/KronRadius.h"
#include "SEImplementation/Plugin/KronRadius/KronRadiusTaskFactory.h"
#include "SEImplementation/Plugin/KronRadius/KronRadiusPlugin.h"

namespace SExtractor {

static StaticPlugin<KronRadiusPlugin> kron_radius_plugin;

void KronRadiusPlugin::registerPlugin(PluginAPI& plugin_api) {
  plugin_api.getTaskFactoryRegistry().registerTaskFactory<KronRadiusTaskFactory, KronRadius>();

  plugin_api.getOutputRegistry().registerColumnConverter<KronRadius, double>(
          "kron_radius",
          [](const KronRadius& prop){
            return prop.getKronRadius();
          }
  );

  plugin_api.getOutputRegistry().registerColumnConverter<KronRadius, int64_t>(
          "kron_flag",
          [](const KronRadius& prop){
            return prop.getFlag();
          }
  );

  // register as optional output (to have it in the output catalog)
  plugin_api.getOutputRegistry().optionalOutput<KronRadius>("KronRadius");
}

std::string KronRadiusPlugin::getIdString() const {
  return "KronRadiusPlugin";
}

}

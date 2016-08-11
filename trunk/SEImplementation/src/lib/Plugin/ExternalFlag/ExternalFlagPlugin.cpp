/*
 * ExternalFlag.cpp
 *
 *  Created on: Aug 5, 2016
 *      Author: mschefer
 */

#include "SEFramework/Plugin/StaticPlugin.h"

#include "SEImplementation/Plugin/ExternalFlag/ExternalFlag.h"
#include "SEImplementation/Plugin/ExternalFlag/ExternalFlagTaskFactory.h"

#include "SEImplementation/Plugin/ExternalFlag/ExternalFlagPlugin.h"

namespace SExtractor {

static StaticPlugin<ExternalFlagPlugin> pixel_boundaries_plugin;

void ExternalFlagPlugin::registerPlugin(PluginAPI& plugin_api) {
  plugin_api.getTaskFactoryRegistry().registerTaskFactory<ExternalFlag>(
      std::unique_ptr<TaskFactory>(new ExternalFlagTaskFactory));

  plugin_api.getOutputRegistry().registerColumnConverter<ExternalFlag, std::int64_t>(
          "IMAFLAGS_ISO",
          [](const ExternalFlag& prop){
            return prop.getFlag();
          }
  );

  plugin_api.getOutputRegistry().registerColumnConverter<ExternalFlag, int>(
          "NIMAFLAGS_ISO",
          [](const ExternalFlag& prop){
            return prop.getCount();
          }
  );

  // External flags are always in the output if there is configuration for
  // producing them
  plugin_api.getOutputRegistry().enableOutput<ExternalFlag>();
}

std::string ExternalFlagPlugin::getIdString() const {
  return "";
}

}





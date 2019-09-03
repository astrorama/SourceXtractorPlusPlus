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
  plugin_api.getTaskFactoryRegistry().registerTaskFactory<ExternalFlagTaskFactory, ExternalFlag>();

  plugin_api.getOutputRegistry().registerColumnConverter<ExternalFlag, std::int64_t>(
          "isophotal_image_flags",
          [](const ExternalFlag& prop){
            return prop.getFlag();
          },
          "[]",
          "Flags for the isophotal magnitude"
  );

  plugin_api.getOutputRegistry().registerColumnConverter<ExternalFlag, int>(
          "isophotal_image_flags_pixel_count",
          [](const ExternalFlag& prop){
            return prop.getCount();
          },
          "[]",
          "Flags provided from input images"
  );

  plugin_api.getOutputRegistry().enableOutput<ExternalFlag>(ExternalFlagTaskFactory::propertyName);
}

std::string ExternalFlagPlugin::getIdString() const {
  return "";
}

}

/*
 * AssocModePlugin.cpp
 *
 *  Created on: Mar 5, 2021
 *      Author: mschefer
 */

#include "SEFramework/Plugin/StaticPlugin.h"

#include "SEImplementation/Plugin/AssocMode/AssocMode.h"
#include "SEImplementation/Plugin/AssocMode/AssocModeTaskFactory.h"
#include "SEImplementation/Plugin/AssocMode/AssocModePlugin.h"

namespace SourceXtractor {

template <typename T>
using NdArray = Euclid::NdArray::NdArray<T>;

static StaticPlugin<AssocModePlugin> assoc_plugin;

void AssocModePlugin::registerPlugin(PluginAPI& plugin_api) {
  plugin_api.getTaskFactoryRegistry()
    .registerTaskFactory<AssocModeTaskFactory, AssocMode>();

  plugin_api.getOutputRegistry().registerColumnConverter<AssocMode, bool>(
    "assoc_match",
    [](const AssocMode& prop){
      return prop.getMatch();
    },
    "",
    "Assoc match"
  );

  plugin_api.getOutputRegistry().registerColumnConverter<AssocMode, NdArray<SeFloat>>(
    "assoc_values",
    [](const AssocMode &prop) {
      return prop.getAssocValues();
    },
    "",
    "Assoc catalog values"
  );

  plugin_api.getOutputRegistry().enableOutput<AssocMode>("AssocMode");
}

std::string AssocModePlugin::getIdString() const {
  return "AssocModePlugin";
}

}

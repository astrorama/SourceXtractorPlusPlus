/*
 * PeakValuePlugin.cpp
 *
 *  Created on: Feb 9, 2017
 *      Author: mschefer
 */

#include "SEFramework/Plugin/StaticPlugin.h"

#include "SEImplementation/Plugin/PeakValue/PeakValue.h"
#include "SEImplementation/Plugin/PeakValue/PeakValueTaskFactory.h"

#include "SEImplementation/Plugin/PeakValue/PeakValuePlugin.h"

namespace SExtractor {

static StaticPlugin<PeakValuePlugin> peak_value_plugin;

void PeakValuePlugin::registerPlugin(PluginAPI& plugin_api) {
  plugin_api.getTaskFactoryRegistry().registerTaskFactory<PeakValueTaskFactory, PeakValue>();

  plugin_api.getOutputRegistry().registerColumnConverter<PeakValue, double>(
          "peak_value",
          [](const PeakValue& prop){
            return prop.getMaxValue();
          }
  );

  plugin_api.getOutputRegistry().optionalOutput<PeakValue>("PeakValue");
}

std::string PeakValuePlugin::getIdString() const {
  return "PeakValue";
}

}


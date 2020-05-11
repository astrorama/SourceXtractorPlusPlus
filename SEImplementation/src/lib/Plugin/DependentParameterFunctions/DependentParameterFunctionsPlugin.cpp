/*
 * DependentParameterFunctionsPlugin.cpp
 *
 *  Created on: Apr 28, 2020
 *      Author: mschefer
 */

#include "SEFramework/Plugin/StaticPlugin.h"

#include "SEImplementation/Plugin/DependentParameterFunctions/DependentParameterFunctions.h"
#include "SEImplementation/Plugin/DependentParameterFunctions/DependentParameterFunctionsPlugin.h"

namespace SourceXtractor {

static StaticPlugin<DependentParameterFunctionsPlugin> dependent_parameter_functions_plugin;

void DependentParameterFunctionsPlugin::registerPlugin(PluginAPI& plugin_api) {

}

std::string DependentParameterFunctionsPlugin::getIdString() const {
  return "DetectionFrameGroupStamp";
}

}

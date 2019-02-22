/*
 * FlexibleModelFittingPlugin.cpp
 *
 *  Created on: Sep 17, 2018
 *      Author: mschefer
 */

#include "SEFramework/Plugin/StaticPlugin.h"

#include "SEImplementation/Plugin/FlexibleModelFitting/FlexibleModelFitting.h"
#include "SEImplementation/Plugin/FlexibleModelFitting/FlexibleModelFittingTaskFactory.h"

#include "SEImplementation/Plugin/FlexibleModelFitting/FlexibleModelFittingPlugin.h"

namespace SExtractor {

static StaticPlugin<FlexibleModelFittingPlugin> flexible_modelfitting_plugin;

void FlexibleModelFittingPlugin::registerPlugin(PluginAPI& plugin_api) {
  plugin_api.getTaskFactoryRegistry().registerTaskFactory<FlexibleModelFittingTaskFactory, FlexibleModelFitting>();

  plugin_api.getOutputRegistry().registerColumnConverter<FlexibleModelFitting, double>(
          "fmf_reduced_chi_2",
          [](const FlexibleModelFitting& prop) {
            return prop.getReducedChiSquared();
          }
  );

  plugin_api.getOutputRegistry().registerColumnConverter<FlexibleModelFitting, int>(
          "fmf_iterations",
          [](const FlexibleModelFitting& prop) {
            return prop.getIterations();
          }
  );

  plugin_api.getOutputRegistry().enableOutput<FlexibleModelFitting>("FlexibleModelFitting");
}

std::string FlexibleModelFittingPlugin::getIdString() const {
  return "FlexibleModelFitting";
}

}

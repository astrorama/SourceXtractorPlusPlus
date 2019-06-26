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

Elements::Logging model_fitting_logger = Elements::Logging::getLogger("ModelFitting");

static StaticPlugin<FlexibleModelFittingPlugin> flexible_modelfitting_plugin;

void FlexibleModelFittingPlugin::registerPlugin(PluginAPI& plugin_api) {
  plugin_api.getTaskFactoryRegistry().registerTaskFactory<FlexibleModelFittingTaskFactory, FlexibleModelFitting>();

  plugin_api.getOutputRegistry().registerColumnConverter<FlexibleModelFitting, double>(
          "fmf_reduced_chi_2",
          [](const FlexibleModelFitting& prop) {
            return prop.getReducedChiSquared();
          },
          "[]",
          "Reduced chi-square of the model fitting"
  );

  plugin_api.getOutputRegistry().registerColumnConverter<FlexibleModelFitting, int>(
          "fmf_iterations",
          [](const FlexibleModelFitting& prop) {
            return prop.getIterations();
          },
          "[]",
          "Number of iterations in the model fitting"
  );

  plugin_api.getOutputRegistry().registerColumnConverter<FlexibleModelFitting, int64_t>(
          "fmf_flags",
          [](const FlexibleModelFitting& prop) {
            return flags2long(prop.getFlags());
          },
          "[]",
          "Model fitting flags"
  );

  plugin_api.getOutputRegistry().enableOutput<FlexibleModelFitting>("FlexibleModelFitting");
}

std::string FlexibleModelFittingPlugin::getIdString() const {
  return "FlexibleModelFitting";
}

}

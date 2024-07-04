/** Copyright © 2019 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3.0 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */
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

namespace SourceXtractor {

Elements::Logging model_fitting_logger = Elements::Logging::getLogger("ModelFitting");

static StaticPlugin<FlexibleModelFittingPlugin> flexible_modelfitting_plugin;

void FlexibleModelFittingPlugin::registerPlugin(PluginAPI& plugin_api) {
  plugin_api.getTaskFactoryRegistry().registerTaskFactory<FlexibleModelFittingTaskFactory, FlexibleModelFitting>();

  plugin_api.getOutputRegistry().registerColumnConverter<FlexibleModelFitting, double>(
          "fmf_reduced_chi_2",
          [](const FlexibleModelFitting& prop) {
            return prop.getReducedChiSquared();
          },
          "",
          "Reduced chi-square of the model fitting"
  );

  plugin_api.getOutputRegistry().registerColumnConverter<FlexibleModelFitting, int>(
          "fmf_iterations",
          [](const FlexibleModelFitting& prop) {
            return prop.getIterations();
          },
          "",
          "Number of iterations in the model fitting"
  );

  plugin_api.getOutputRegistry().registerColumnConverter<FlexibleModelFitting, int>(
    "fmf_stop_reason",
    [](const FlexibleModelFitting& prop) {
      return prop.getStopReason();
    },
    "",
    "Stop reason (engine dependent)"
  );

  plugin_api.getOutputRegistry().registerColumnConverter<FlexibleModelFitting, float>(
    "fmf_duration",
    [](const FlexibleModelFitting& prop) {
      return prop.getDuration();
    },
    "seconds",
    "Fitting runtime"
  );

  plugin_api.getOutputRegistry().registerColumnConverter<FlexibleModelFitting, int64_t>(
          "fmf_flags",
          [](const FlexibleModelFitting& prop) {
            return flags2long(prop.getFlags());
          },
          "",
          "Model fitting flags"
  );

  plugin_api.getOutputRegistry().registerColumnConverter<FlexibleModelFitting, std::vector<SeFloat>>(
            "fmf_chi2_per_meta",
            [](const FlexibleModelFitting& prop) {
              return prop.getChiSquaredPerMetaIteration();
            },
            "",
            "Reduced chi^2 per meta-iteration"
    );

  plugin_api.getOutputRegistry().registerColumnConverter<FlexibleModelFitting, std::vector<int>>(
            "fmf_iterations_per_meta",
            [](const FlexibleModelFitting& prop) {
              return prop.getIterationsPerMetaIteration();
            },
            "",
            "Iterations per meta-iteration"
    );

  plugin_api.getOutputRegistry().registerColumnConverter<FlexibleModelFitting, int>(
            "fmf_meta_iterations",
            [](const FlexibleModelFitting& prop) {
              return prop.getMetaIterations();
            },
            "",
            "Meta-iterations"
    );

  plugin_api.getOutputRegistry().registerColumnConverter<FlexibleModelFitting, std::vector<SeFloat>>(
            "fmf_fitting_areas_x",
            [](const FlexibleModelFitting& prop) {
              return prop.getFittingAreasX();
            },
            "",
            "Fitting areas X"
    );

  plugin_api.getOutputRegistry().registerColumnConverter<FlexibleModelFitting, std::vector<SeFloat>>(
            "fmf_fitting_areas_y",
            [](const FlexibleModelFitting& prop) {
              return prop.getFittingAreasY();
            },
            "",
            "Fitting areas Y"
    );

  plugin_api.getOutputRegistry().enableOutput<FlexibleModelFitting>("FlexibleModelFitting");
}

std::string FlexibleModelFittingPlugin::getIdString() const {
  return "FlexibleModelFitting";
}

}

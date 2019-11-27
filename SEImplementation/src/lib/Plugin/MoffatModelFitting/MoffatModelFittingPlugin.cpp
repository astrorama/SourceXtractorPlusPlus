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
 * MoffatModelFittingPlugin.cpp
 *
 *  Created on: May 2, 2017
 *      Author: mschefer
 */

#include "SEImplementation/Plugin/MoffatModelFitting/MoffatModelEvaluator.h"
#include "SEImplementation/Plugin/MoffatModelFitting/MoffatModelFitting.h"
#include "SEImplementation/Plugin/MoffatModelFitting/MoffatModelFittingPlugin.h"
#include "SEImplementation/Plugin/MoffatModelFitting/MoffatModelFittingTaskFactory.h"
#include "SEFramework/Plugin/StaticPlugin.h"


namespace SourceXtractor {

static StaticPlugin<MoffatModelFittingPlugin> simple_modelfitting_plugin;

void MoffatModelFittingPlugin::registerPlugin(PluginAPI& plugin_api) {
  plugin_api.getTaskFactoryRegistry().registerTaskFactory<MoffatModelFittingTaskFactory, MoffatModelFitting>();
  plugin_api.getTaskFactoryRegistry().registerTaskFactory<MoffatModelFittingTaskFactory, MoffatModelEvaluator>();

  plugin_api.getOutputRegistry().registerColumnConverter<MoffatModelFitting, double>(
          "smf_x",
          [](const MoffatModelFitting& prop) {
            return prop.getX() + 1.0;
          },
          "[pixel]",
          "X-position of the Moffat fit"
  );

  plugin_api.getOutputRegistry().registerColumnConverter<MoffatModelFitting, double>(
          "smf_y",
          [](const MoffatModelFitting& prop) {
            return prop.getY() + 1.0;
          },
          "[pixel]",
          "Y-position of the Moffat fit"
  );

  plugin_api.getOutputRegistry().registerColumnConverter<MoffatModelFitting, int>(
          "smf_iter",
          [](const MoffatModelFitting& prop){
            return prop.getIterations();
          },
          "",
          "Number of iterations in the Moffat fitting"
  );

  plugin_api.getOutputRegistry().enableOutput<MoffatModelFitting>("MoffatModelFitting");
}

std::string MoffatModelFittingPlugin::getIdString() const {
  return "MoffatModelFitting";
}

}

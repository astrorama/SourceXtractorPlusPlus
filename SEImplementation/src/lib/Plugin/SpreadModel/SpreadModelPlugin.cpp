/** Copyright © 2019-2026 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
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
#include "SEImplementation/Plugin/SpreadModel/SpreadModel.h"
#include "SEImplementation/Plugin/SpreadModel/SpreadModelPlugin.h"
#include "SEImplementation/Plugin/SpreadModel/SpreadModelTaskFactory.h"
#include "SEFramework/Plugin/StaticPlugin.h"

namespace SourceXtractor {

static StaticPlugin<SpreadModelPlugin> spread_model_plugin;

void SpreadModelPlugin::registerPlugin(PluginAPI& plugin_api) {
  plugin_api.getTaskFactoryRegistry().registerTaskFactory<SpreadModelTaskFactory, SpreadModel>();

  plugin_api.getOutputRegistry().registerColumnConverter<SpreadModel, double>(
          "spread_model",
          [](const SpreadModel& prop) {
            return prop.getSpreadModel();
          },
          "",
          "Spread model"
  );

  plugin_api.getOutputRegistry().registerColumnConverter<SpreadModel, double>(
          "spread_model_error",
          [](const SpreadModel& prop) {
            return prop.getSpreadModelError();
          },
          "",
          "Spread model error"
  );

  plugin_api.getOutputRegistry().enableOutput<SpreadModel>("SpreadModel");
}

std::string SpreadModelPlugin::getIdString() const {
  return "SpreadModel";
}

}
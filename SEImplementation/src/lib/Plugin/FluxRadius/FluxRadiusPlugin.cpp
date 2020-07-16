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

#include "SEFramework/Plugin/StaticPlugin.h"
#include "SEImplementation/Plugin/FluxRadius/FluxRadius.h"
#include "SEImplementation/Plugin/FluxRadius/FluxRadiusPlugin.h"
#include "SEImplementation/Plugin/FluxRadius/FluxRadiusTaskFactory.h"

using namespace Euclid::NdArray;

namespace SourceXtractor {

static StaticPlugin<FluxRadiusPlugin> flux_radius_plugin;

std::string FluxRadiusPlugin::getIdString() const {
  return "FluxRadiusPlugin";
}

void FluxRadiusPlugin::registerPlugin(PluginAPI& plugin_api) {
  plugin_api.getTaskFactoryRegistry().registerTaskFactory<FluxRadiusTaskFactory, FluxRadius>();

  plugin_api.getOutputRegistry().registerColumnConverter<FluxRadius, NdArray<SeFloat>>(
    "flux_radius",
    [](const FluxRadius& prop){
      return prop.getFluxRadius();
    },
    "pixel",
    "Radius containing a fraction of the flux"
  );

  plugin_api.getOutputRegistry().enableOutput<FluxRadius>("FluxRadius");
}

} // end of namespace SourceXtractor

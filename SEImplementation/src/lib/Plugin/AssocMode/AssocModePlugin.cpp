/** Copyright © 2021 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
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

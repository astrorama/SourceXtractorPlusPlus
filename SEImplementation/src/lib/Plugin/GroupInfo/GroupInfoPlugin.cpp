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
 * GroupInfoPlugin.cpp
 *
 *  Created on: 2019 M01 30
 *      Author: mschefer
 */


#include "SEFramework/Plugin/StaticPlugin.h"

#include "SEImplementation/Plugin/GroupInfo/GroupInfo.h"
#include "SEImplementation/Plugin/GroupInfo/GroupInfoTaskFactory.h"

#include "SEImplementation/Plugin/GroupInfo/GroupInfoPlugin.h"

namespace SourceXtractor {

static StaticPlugin<GroupInfoPlugin> group_info_plugin;

void GroupInfoPlugin::registerPlugin(PluginAPI& plugin_api) {
  plugin_api.getTaskFactoryRegistry().registerTaskFactory<GroupInfoTaskFactory, GroupInfo>();

  plugin_api.getOutputRegistry().registerColumnConverter<GroupInfo, double>(
      "group_id",
      [](const GroupInfo& prop){
        return prop.getGroupId();
      },
      "",
      "Running group number"
  );

  plugin_api.getOutputRegistry().enableOutput<GroupInfo>("GroupInfo");
}

std::string GroupInfoPlugin::getIdString() const {
  return "GroupInfo";
}

}


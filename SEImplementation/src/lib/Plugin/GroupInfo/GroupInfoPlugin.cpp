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

namespace SExtractor {

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


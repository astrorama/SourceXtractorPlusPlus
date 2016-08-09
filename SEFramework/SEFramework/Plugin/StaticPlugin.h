/*
 * StaticPlugin.h
 *
 *  Created on: Aug 5, 2016
 *      Author: mschefer
 */

#ifndef _SEFRAMEWORK_PLUGIN_STATICPLUGIN_H_
#define _SEFRAMEWORK_PLUGIN_STATICPLUGIN_H_

#include "SEFramework/Plugin/Plugin.h"
#include "SEFramework/Plugin/PluginManager.h"

namespace SExtractor {

template<typename PluginType>
class StaticPlugin {
public:
  StaticPlugin() {
    static_assert(std::is_base_of<Plugin, PluginType>::value, "PluginType must inherit from SExtractor::Plugin");
    PluginManager::registerStaticPlugin<PluginType>();
  }
};

}



#endif /* _SEFRAMEWORK_PLUGIN_STATICPLUGIN_H_ */

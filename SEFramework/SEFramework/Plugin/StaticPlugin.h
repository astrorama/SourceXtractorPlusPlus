/*
 * StaticPlugin.h
 *
 *  Created on: Aug 5, 2016
 *      Author: mschefer
 */

#ifndef _SEFRAMEWORK_PLUGIN_STATICPLUGIN_H_
#define _SEFRAMEWORK_PLUGIN_STATICPLUGIN_H_

#include <iostream> // fixme tmp

#include "SEFramework/Plugin/PluginManager.h"

namespace SExtractor {

template<typename T>
class StaticPlugin {
public:
  StaticPlugin() {
    std::cout << "StaticPlugin" << std::endl;
    PluginManager::registerStaticPlugin<T>();
  }
};

}



#endif /* _SEFRAMEWORK_PLUGIN_STATICPLUGIN_H_ */

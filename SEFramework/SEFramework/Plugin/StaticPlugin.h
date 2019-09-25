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

/**
 * @class StaticPlugin
 * @brief Used to register compile-time (static) plugins with the PluginManager
 */

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

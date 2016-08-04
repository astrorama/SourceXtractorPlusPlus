/*
 * PluginManager.cpp
 *
 *  Created on: Jul 28, 2016
 *      Author: mschefer
 */

#include <iostream>

#include <boost/dll/import.hpp>

#include "Configuration/ConfigManager.h"

#include "SEFramework/Configuration/PluginConfig.h"

#include "SEFramework/Task/TaskFactoryRegistry.h"

#include "SEFramework/Plugin/PluginManager.h"
#include "SEFramework/Plugin/Plugin.h"

namespace SExtractor {


void PluginManager::reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const {
  manager.registerConfiguration<PluginConfig>();
}

void PluginManager::configure(Euclid::Configuration::ConfigManager& manager) {
  auto& plugin_config = manager.getConfiguration<PluginConfig>();
  auto plugin_paths = plugin_config.getPluginPaths();

  for (auto& plugin_path : plugin_paths) {
    typedef std::shared_ptr<Plugin> (pluginapi_create_t)();
    std::function<pluginapi_create_t> creator;

    boost::dll::shared_library lib(plugin_path);
    creator = lib.get_alias<pluginapi_create_t>("create_plugin");

    auto plugin = creator();
    auto id_string = plugin->getIdString();
    std::cout << id_string << std::endl;

    plugin->registerPlugin(*this);

    // keep the library loaded while PluginManager still exists
    m_loaded_plugins.push_back(lib);
  }
}

}

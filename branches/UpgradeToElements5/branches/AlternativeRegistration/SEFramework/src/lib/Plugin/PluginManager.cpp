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

#include "SEFramework/Plugin/PluginManager.h"
#include "SEFramework/Plugin/Plugin.h"

namespace SExtractor {


void PluginManager::reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) {
  manager.registerConfiguration<PluginConfig>();
}

void PluginManager::configure(Euclid::Configuration::ConfigManager& manager) {
  auto& plugin_config = manager.getConfiguration<PluginConfig>();

  auto plugin_paths = plugin_config.getPluginPaths();

  for (auto& plugin_path : plugin_paths) {
    typedef std::shared_ptr<Plugin> (pluginapi_create_t)();
    std::function<pluginapi_create_t> creator;

    creator = boost::dll::import_alias<pluginapi_create_t>(
        plugin_path, "create_plugin", boost::dll::load_mode::append_decorations);

    auto plugin = creator();
    auto id_string = plugin->getIdString();
    std::cout << id_string << std::endl;

    plugin->registerPlugin(*this);

    m_loaded_plugins.push_back(plugin);
  }
}

void PluginManager::registerTaskFactory(std::unique_ptr<TaskFactory> task_factory) {
  RegistrationManager::instance().registerObject(std::move(task_factory));
}


}

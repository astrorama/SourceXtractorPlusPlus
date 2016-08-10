/*
 * PluginManager.cpp
 *
 *  Created on: Jul 28, 2016
 *      Author: mschefer
 */

#include <iostream>
#include <string>
#include <fstream>

#include <boost/dll/import.hpp>
#include <boost/filesystem/fstream.hpp>

#include "Configuration/ConfigManager.h"

#include "SEFramework/Plugin/Plugin.h"

#include "SEFramework/Plugin/PluginManager.h"

namespace SExtractor {

static const std::string PLUGIN_DIRECTORY_ENV_VAR { "SEXTRACTOR_PLUGIN_PATH" };
static const std::string PLUGIN_LIST_FILENAME { "SEPluginList.cfg" };

std::vector<std::unique_ptr<Plugin>> PluginManager::s_static_plugins;

void PluginManager::loadPlugins() {
  // load staticly registered plugins
  for (auto& static_plugin : s_static_plugins) {
    static_plugin->registerPlugin(*this);
  }

  // load dynamic plugins
  auto plugin_paths = getPluginPaths();
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

std::vector<boost::filesystem::path> PluginManager::getPluginPaths() const {
  std::vector<boost::filesystem::path> plugin_paths;

  auto plugin_path_env_variable = std::getenv(PLUGIN_DIRECTORY_ENV_VAR.c_str());
  if (plugin_path_env_variable != nullptr) {
    boost::filesystem::path plugin_path(plugin_path_env_variable);
    if (boost::filesystem::is_directory(plugin_path)) {

      auto plugin_list_path = plugin_path / PLUGIN_LIST_FILENAME;
      boost::filesystem::ifstream plugin_list_file(plugin_list_path);

      std::vector<std::string> plugin_names;

      std::copy(std::istream_iterator<std::string>(plugin_list_file),
                std::istream_iterator<std::string>(),
                std::back_inserter(plugin_names));

      for (auto& plugin_name : plugin_names) {
        auto full_path = plugin_path / boost::filesystem::path(plugin_name);
        full_path += boost::dll::shared_library::suffix();

        if (boost::filesystem::exists(full_path)) {
          std::cout << "Loading plugin: " << plugin_name << std::endl;
          plugin_paths.emplace_back(full_path);
        }
      }
    }
  }

  return plugin_paths;
}

}

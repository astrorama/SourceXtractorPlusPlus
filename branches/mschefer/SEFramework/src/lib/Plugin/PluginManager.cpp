/*
 * PluginManager.cpp
 *
 *  Created on: Jul 28, 2016
 *      Author: mschefer
 */

#include "SEFramework/Plugin/PluginManager.h"

#include <iostream>
#include <string>
#include <fstream>

#if USE_BOOST_DLL
#include <boost/dll/import.hpp>
#endif
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include "ElementsKernel/Logging.h"
#include "Configuration/ConfigManager.h"

#include "SEFramework/Plugin/Plugin.h"

namespace SExtractor {

std::vector<std::unique_ptr<Plugin>> PluginManager::s_static_plugins;

static Elements::Logging logger = Elements::Logging::getLogger("PluginManager");

#if USE_BOOST_DLL
static std::vector<boost::filesystem::path> getPluginPaths(
                const std::string& plugin_path_str,
                const std::vector<std::string>& plugin_list) {
  std::vector<boost::filesystem::path> plugin_paths;

  if (plugin_path_str != "") {
    boost::filesystem::path plugin_path(plugin_path_str);
    if (boost::filesystem::is_directory(plugin_path)) {
      for (auto& plugin_name : plugin_list) {
        auto full_path = plugin_path / boost::filesystem::path(plugin_name);
        full_path += boost::dll::shared_library::suffix();

        if (boost::filesystem::exists(full_path)) {
          logger.info() << "Loading plugin from file: " << full_path;
          plugin_paths.emplace_back(full_path);
        } else {
          logger.warn() << "Failed to load plugins from " << full_path << " - file not found";
        }
      }
    } else {
      logger.warn() << "The " << plugin_path << " is not a directory. No plugins will be loaded";
    }
  }

  return plugin_paths;
}
#endif

void PluginManager::loadPlugins() {
  // load staticly registered plugins
  for (auto& static_plugin : s_static_plugins) {
    static_plugin->registerPlugin(*this);
  }

#if USE_BOOST_DLL
  // load dynamic plugins
  auto plugin_paths = getPluginPaths(m_plugin_path, m_plugin_list);
  for (auto& plugin_path : plugin_paths) {
    typedef std::shared_ptr<Plugin> (pluginapi_create_t)();
    std::function<pluginapi_create_t> creator;

    boost::dll::shared_library lib(plugin_path);
    creator = lib.get_alias<pluginapi_create_t>("create_plugin");

    auto plugin = creator();
    auto id_string = plugin->getIdString();
    logger.info() << "Registering plugin " << id_string;

    plugin->registerPlugin(*this);

    // keep the library loaded while PluginManager still exists
    m_loaded_plugins.push_back(lib);
  }
#endif
}

}

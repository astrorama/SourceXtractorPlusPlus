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

namespace SourceXtractor {

std::vector<std::unique_ptr<Plugin>> PluginManager::s_static_plugins;

static Elements::Logging logger = Elements::Logging::getLogger("PluginManager");

#if USE_BOOST_DLL
std::vector<boost::dll::shared_library> PluginManager::s_loaded_plugins;
#endif

void PluginManager::loadPlugins() {
  // load staticly registered plugins
  for (auto& static_plugin : s_static_plugins) {
    static_plugin->registerPlugin(*this);
  }

#if USE_BOOST_DLL
  typedef std::shared_ptr<Plugin> (pluginapi_create_t)();

  // load dynamic plugins
  auto load_mode = boost::dll::load_mode::append_decorations | boost::dll::load_mode::search_system_folders;
  for (auto& plugin_name : m_plugin_list) {
    boost::dll::shared_library lib;

    // Try on the explicit path first
    if (!m_plugin_path.empty()) {
      auto full_path = m_plugin_path / plugin_name;
      try {
        lib.load(full_path, load_mode);
      } catch (const boost::system::system_error&) {
        // ignore
      }
    }
    // Try on the system path. This time, propagate the failure
    if (!lib.is_loaded()) {
      lib.load(plugin_name, load_mode);
    }

    auto creator = lib.get_alias<pluginapi_create_t>("create_plugin");
    auto plugin = creator();
    auto id_string = plugin->getIdString();
    logger.info() << "Registering plugin " << id_string;
    plugin->registerPlugin(*this);
    s_loaded_plugins.push_back(lib);
  }

#endif
}

}

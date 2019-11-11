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
 * PluginManager.h
 *
 *  Created on: Jul 27, 2016
 *      Author: mschefer
 */

#ifndef _SEFRAMEWORK_PLUGIN_PLUGINMANAGER_H_
#define _SEFRAMEWORK_PLUGIN_PLUGINMANAGER_H_

#include <boost/version.hpp>
#define USE_BOOST_DLL BOOST_VERSION >= 105500

#if USE_BOOST_DLL
#include <boost/dll/shared_library.hpp>
#endif

#include <memory>
#include <vector>

#include "SEFramework/Configuration/Configurable.h"
#include "SEFramework/Task/TaskFactoryRegistry.h"
#include "SEFramework/Plugin/PluginAPI.h"

namespace SourceXtractor {

class Plugin;

/**
 * @class PluginManager
 * @brief PluginManager handles the loading of plugins and calls their registration function, providing them with
 *        with a PluginAPI implementation.
 */


class PluginManager : public PluginAPI {
public:

  virtual ~PluginManager() = default;

  PluginManager(std::shared_ptr<TaskFactoryRegistry> task_factory_registry,
                std::shared_ptr<OutputRegistry> output_registry,
                long config_manager_id,
                std::string plugin_path,
                std::vector<std::string> plugin_list) :
        m_plugin_path(plugin_path),
        m_plugin_list(plugin_list),
        m_task_factory_registry(task_factory_registry),
        m_output_registry(output_registry),
        m_config_manager_id(config_manager_id) {}

  /// loads all the available plugins. Both those linked at compile-time and those loaded at run-time
  void loadPlugins();

  // PluginAPI implementation
  virtual TaskFactoryRegistry& getTaskFactoryRegistry() const override {
    return *m_task_factory_registry;
  }

  virtual OutputRegistry& getOutputRegistry() const override {
    return *m_output_registry;
  }

  Euclid::Configuration::ConfigManager& getConfigManager() const override {
    return Euclid::Configuration::ConfigManager::getInstance(m_config_manager_id);
  }

  /// registers a plugin, this is used to register plugins linked at compile-time
  template<typename T>
  static void registerStaticPlugin() {
    s_static_plugins.emplace_back(new T);
  }

private:
  std::string m_plugin_path;
  std::vector<std::string> m_plugin_list;

  std::shared_ptr<TaskFactoryRegistry> m_task_factory_registry;
  std::shared_ptr<OutputRegistry> m_output_registry;
  long m_config_manager_id;

#if USE_BOOST_DLL
  /// @details
  /// Unfortunately, plugins can register themselves in multiple places:
  /// OutputRegistry, TaskRegistry, ConfigurationManager...
  /// For the former two we could somehow handle the lifetime better, but the later
  /// comes for Alexandria, and does not allow to unregister, nor override the registration methods.
  /// We need to make sure the external plugins survive all those classes, or on destruction
  /// the process will segfault when trying to call the virtual destructor that is on an unloaded
  /// library.
  static std::vector<boost::dll::shared_library> s_loaded_plugins;
#endif
  static std::vector<std::unique_ptr<Plugin>> s_static_plugins;
};

}

#endif /* _SEFRAMEWORK_PLUGIN_PLUGINMANAGER_H_ */

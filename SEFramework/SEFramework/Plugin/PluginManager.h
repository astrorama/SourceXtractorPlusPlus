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

namespace SExtractor {

class Plugin;

class PluginManager : public PluginAPI {
public:

  virtual ~PluginManager() = default;

  PluginManager(std::shared_ptr<TaskFactoryRegistry> task_factory_registry,
                std::shared_ptr<OutputRegistry> output_registry,
                std::string plugin_path,
                std::vector<std::string> plugin_list) :
        m_plugin_path(plugin_path),
        m_plugin_list(plugin_list),
        m_task_factory_registry(task_factory_registry),
        m_output_registry(output_registry) {}

  void loadPlugins();

  // PluginAPI implementation
  virtual TaskFactoryRegistry& getTaskFactoryRegistry() const override {
    return *m_task_factory_registry;
  }

  virtual OutputRegistry& getOutputRegistry() const override {
    return *m_output_registry;
  }

  template<typename T>
  static void registerStaticPlugin() {
    s_static_plugins.emplace_back(new T);
  }

private:
  
  std::string m_plugin_path;
  std::vector<std::string> m_plugin_list;
#if USE_BOOST_DLL
  std::vector<boost::dll::shared_library> m_loaded_plugins;
#endif
  
  std::shared_ptr<TaskFactoryRegistry> m_task_factory_registry;
  std::shared_ptr<OutputRegistry> m_output_registry;

  static std::vector<std::unique_ptr<Plugin>> s_static_plugins;
};

}

#endif /* _SEFRAMEWORK_PLUGIN_PLUGINMANAGER_H_ */

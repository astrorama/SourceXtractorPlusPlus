/*
 * PluginManager.h
 *
 *  Created on: Jul 27, 2016
 *      Author: mschefer
 */

#ifndef _SEFRAMEWORK_PLUGIN_PLUGINMANAGER_H_
#define _SEFRAMEWORK_PLUGIN_PLUGINMANAGER_H_

#include <boost/dll/shared_library.hpp>

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
                std::shared_ptr<OutputRegistry> output_registry) :
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
  std::vector<boost::filesystem::path> getPluginPaths() const;

  std::vector<boost::dll::shared_library> m_loaded_plugins;
  std::shared_ptr<TaskFactoryRegistry> m_task_factory_registry;
  std::shared_ptr<OutputRegistry> m_output_registry;

  static std::vector<std::unique_ptr<Plugin>> s_static_plugins;
};

}

#endif /* _SEFRAMEWORK_PLUGIN_PLUGINMANAGER_H_ */

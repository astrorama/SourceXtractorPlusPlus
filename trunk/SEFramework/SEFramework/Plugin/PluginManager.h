/*
 * PluginManager.h
 *
 *  Created on: Jul 27, 2016
 *      Author: mschefer
 */

#ifndef _SEFRAMEWORK_PLUGIN_PLUGINMANAGER_H_
#define _SEFRAMEWORK_PLUGIN_PLUGINMANAGER_H_

#include <memory>
#include <vector>

#include "SEFramework/Configuration/Configurable.h"

namespace SExtractor {

class Plugin;
class TaskFactory;

class PluginManager : public Configurable {
public:

  virtual ~PluginManager() = default;

  virtual void registerTaskFactory(std::unique_ptr<TaskFactory> task_factory);

  // Implements the Configurable interface
  virtual void reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) override;
  virtual void configure(Euclid::Configuration::ConfigManager& manager) override;

private:
  std::vector<std::shared_ptr<Plugin>> m_loaded_plugins;

};

}

#endif /* _SEFRAMEWORK_PLUGIN_PLUGINMANAGER_H_ */

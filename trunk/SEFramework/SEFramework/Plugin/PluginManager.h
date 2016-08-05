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

#include "SEFramework/Registration/RegistrationManager.h"
#include "SEFramework/Configuration/Configurable.h"
#include "SEFramework/Task/TaskFactoryRegistry.h"
#include "SEFramework/Plugin/PluginAPI.h"

namespace SExtractor {

class Plugin;

class PluginManager : public Configurable, public PluginAPI {
public:

  virtual ~PluginManager() = default;

  // Implements the Configurable interface
  virtual void reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const override;
  virtual void configure(Euclid::Configuration::ConfigManager& manager) override;

  // PluginAPI implementation
  virtual TaskFactoryRegistry& getTaskFactoryRegistry() const override {
    return *RegistrationManager::instance().getTaskFactoryRegistry();
  }

  virtual OutputRegistry& getOutputRegistry() const override {
    return output_registry;
  }

private:
  std::vector<boost::dll::shared_library> m_loaded_plugins;

};

}

#endif /* _SEFRAMEWORK_PLUGIN_PLUGINMANAGER_H_ */

/*
 * PluginAPI.h
 *
 *  Created on: Aug 5, 2016
 *      Author: mschefer
 */

#ifndef _SEFRAMEWORK_PLUGIN_PLUGINAPI_H_
#define _SEFRAMEWORK_PLUGIN_PLUGINAPI_H_

#include "Configuration/ConfigManager.h"
#include "SEFramework/Task/TaskFactoryRegistry.h"
#include "SEFramework/Output/OutputRegistry.h"

namespace SExtractor {

/**
 * @class PluginAPI
 * @brief This interface is given to the plugin to let it access object instances from the framework.
 */


class PluginAPI {
public:
  virtual ~PluginAPI() = default;

  virtual TaskFactoryRegistry& getTaskFactoryRegistry() const = 0;
  virtual OutputRegistry& getOutputRegistry() const = 0;
  virtual Euclid::Configuration::ConfigManager& getConfigManager() const = 0;
};

}

#endif /* _SEFRAMEWORK_PLUGIN_PLUGINAPI_H_ */

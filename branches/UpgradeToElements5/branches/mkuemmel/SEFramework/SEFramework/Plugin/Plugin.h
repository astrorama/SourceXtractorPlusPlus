/*
 * Plugin.h
 *
 *  Created on: Jul 26, 2016
 *      Author: mschefer
 */

#ifndef _SEFRAMEWORK_PLUGIN_PLUGIN_H_
#define _SEFRAMEWORK_PLUGIN_PLUGIN_H_

#include <SEFramework/Plugin/PluginAPI.h>

namespace SExtractor {

/**
 * @class Plugin
 * @brief Plugins must implement this interface.
 *
 * @details registerPlugin() is going to be called to give the plugin a chance to register what it needs to.
 */

class Plugin {
public:
  virtual ~Plugin() = default;

  virtual std::string getIdString() const = 0;
  virtual void registerPlugin(PluginAPI& plugin_api) = 0;
};

} // namespace SExtractor

#endif /* _SEFRAMEWORK_PLUGIN_PLUGIN_H_ */

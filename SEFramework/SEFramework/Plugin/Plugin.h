/*
 * Plugin.h
 *
 *  Created on: Jul 26, 2016
 *      Author: mschefer
 */

#ifndef _SEFRAMEWORK_PLUGIN_PLUGIN_H_
#define _SEFRAMEWORK_PLUGIN_PLUGIN_H_

#include <vector>

#include <SEFramework/Task/TaskFactory.h>

namespace SExtractor {

class PluginManager;

class Plugin {
public:
  virtual std::string getIdString() const = 0;
  virtual void registerPlugin(PluginManager& plugin_manager) = 0;
};

} // namespace SExtractor

#endif /* _SEFRAMEWORK_PLUGIN_PLUGIN_H_ */

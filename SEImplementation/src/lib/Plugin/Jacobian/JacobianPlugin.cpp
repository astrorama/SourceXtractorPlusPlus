/*
 * JacobianPlugin.cpp
 *
 *  Created on: Oct 08, 2018
 *      Author: Alejandro Alvarez Ayllon
 */

#include "SEFramework/Plugin/StaticPlugin.h"
#include "SEImplementation/Plugin/Jacobian/Jacobian.h"
#include "SEImplementation/Plugin/Jacobian/JacobianTaskFactory.h"
#include "SEImplementation/Plugin/Jacobian/JacobianPlugin.h"

namespace SExtractor {

static StaticPlugin<JacobianPlugin> jacobian_plugin;

std::string JacobianPlugin::getIdString() const {
  return "JacobianPlugin";
}

void JacobianPlugin::registerPlugin(PluginAPI &plugin_api) {
  plugin_api.getTaskFactoryRegistry().registerTaskFactory<JacobianTaskFactory, JacobianSource, JacobianGroup>();
}

} // end SExtractor

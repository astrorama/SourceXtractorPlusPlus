/*
 * JacobianPlugin.h
 *
 *  Created on: Oct 08, 2018
 *      Author: Alejandro Alvarez Ayllon
 */


#ifndef _SEIMPLEMENTATION_PLUGIN_JACOBIAN_JACOBIANPLUGIN_H_
#define _SEIMPLEMENTATION_PLUGIN_JACOBIAN_JACOBIANPLUGIN_H_

#include "SEFramework/Plugin/Plugin.h"

namespace SExtractor {

class JacobianPlugin: public Plugin {
public:
  virtual ~JacobianPlugin() = default;

  std::string getIdString() const override;
  void registerPlugin(PluginAPI& plugin_api) override;
};

} // end SExtractor

#endif // _SEIMPLEMENTATION_PLUGIN_JACOBIAN_JACOBIANPLUGIN_H_

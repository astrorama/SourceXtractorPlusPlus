/*
 * PointModelFittingPlugin.h
 *
 *  Created on: July 11, 2018
 *      Author: Alejandro Álvarez
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_POINTMODELFITTING_POINTMODELFITTINGPLUGIN_H_
#define _SEIMPLEMENTATION_PLUGIN_POINTMODELFITTING_POINTMODELFITTINGPLUGIN_H_

#include "SEFramework/Plugin/Plugin.h"

namespace SExtractor {

class PointModelFittingPlugin: public Plugin {
public:
  virtual ~PointModelFittingPlugin() = default;

  virtual void registerPlugin(PluginAPI& plugin_api) override;
  virtual std::string getIdString() const override;
};

} // SExtractor

#endif /* _SEIMPLEMENTATION_PLUGIN_POINTMODELFITTING_POINTMODELFITTINGPLUGIN_H_ */

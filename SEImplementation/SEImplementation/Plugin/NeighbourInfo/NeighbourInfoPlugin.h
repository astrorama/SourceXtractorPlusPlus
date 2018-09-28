/*
 * NeighbourInfo.h
 *
 *  Created on: Sep 28, 2018
 *      Author: Alejandro Alvarez Ayllon
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_NEIGHBOURINFO_NEIGHBOURINFOPLUGIN_H_
#define _SEIMPLEMENTATION_PLUGIN_NEIGHBOURINFO_NEIGHBOURINFOPLUGIN_H_

#include "SEFramework/Plugin/Plugin.h"

namespace SExtractor {

class NeighbourInfoPlugin: public Plugin {
public:
  virtual ~NeighbourInfoPlugin() = default;

  virtual void registerPlugin(PluginAPI& plugin_api) override;
  virtual std::string getIdString() const override;
};

} // end SExtractor

#endif // _SEIMPLEMENTATION_PLUGIN_NEIGHBOURINFO_NEIGHBOURINFOPLUGIN_H_

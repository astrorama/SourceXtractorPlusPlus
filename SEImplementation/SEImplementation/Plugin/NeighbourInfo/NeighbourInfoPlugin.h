/*
 * NeighbourInfo.h
 *
 *  Created on: Oct 02, 2018
 *      Author: Alejandro Alvarez Ayllon
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_DETECTIONNEIGHBOURINFO_DETECTIONNEIGHBOURINFOPLUGIN_H_
#define _SEIMPLEMENTATION_PLUGIN_DETECTIONNEIGHBOURINFO_DETECTIONNEIGHBOURINFOPLUGIN_H_

#include "SEFramework/Plugin/Plugin.h"

namespace SExtractor {

class NeighbourInfoPlugin: public Plugin {
public:
  virtual ~NeighbourInfoPlugin() = default;

  virtual void registerPlugin(PluginAPI& plugin_api) override;
  virtual std::string getIdString() const override;
};

} // end SExtractor

#endif // _SEIMPLEMENTATION_PLUGIN_DETECTIONNEIGHBOURINFO_DETECTIONNEIGHBOURINFOPLUGIN_H_

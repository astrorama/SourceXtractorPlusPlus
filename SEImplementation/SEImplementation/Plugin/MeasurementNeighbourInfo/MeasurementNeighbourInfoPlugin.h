/*
 * MeasurementNeighbourInfo.h
 *
 *  Created on: Sep 28, 2018
 *      Author: Alejandro Alvarez Ayllon
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_MEASUREMENTNEIGHBOURINFO_MEASUREMENTNEIGHBOURINFOPLUGIN_H_
#define _SEIMPLEMENTATION_PLUGIN_MEASUREMENTNEIGHBOURINFO_MEASUREMENTNEIGHBOURINFOPLUGIN_H_

#include "SEFramework/Plugin/Plugin.h"

namespace SExtractor {

class MeasurementNeighbourInfoPlugin: public Plugin {
public:
  virtual ~MeasurementNeighbourInfoPlugin() = default;

  virtual void registerPlugin(PluginAPI& plugin_api) override;
  virtual std::string getIdString() const override;
};

} // end SExtractor

#endif // _SEIMPLEMENTATION_PLUGIN_MEASUREMENTNEIGHBOURINFO_MEASUREMENTNEIGHBOURINFOPLUGIN_H_

/*
 * MeasurementFrameRectanglePlugin.h
 *
 *  Created on: Sep 24, 2018
 *      Author: Alejandro Alvarez Ayllon
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_MEASUREMENTFRAMERECTANGLE_MEASUREMENTFRAMERECTANGLEPLUGIN_H_
#define _SEIMPLEMENTATION_PLUGIN_MEASUREMENTFRAMERECTANGLE_MEASUREMENTFRAMERECTANGLEPLUGIN_H_

#include "SEFramework/Plugin/Plugin.h"

namespace SExtractor {

class MeasurementFrameRectanglePlugin : public Plugin {

public:

  virtual ~MeasurementFrameRectanglePlugin() = default;

  virtual void registerPlugin(PluginAPI& plugin_api) override;
  virtual std::string getIdString() const override;

private:

};

} // end SExtractor

#endif // _SEIMPLEMENTATION_PLUGIN_MEASUREMENTFRAMERECTANGLE_MEASUREMENTFRAMERECTANGLEPLUGIN_H_

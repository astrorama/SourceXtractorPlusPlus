/*
 * MeasurementFrameGroupRectanglePlugin.h
 *
 *  Created on: Sep 5, 2018
 *      Author: Alejandro Alvarez Ayllon
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_MEASUREMENTFRAMEGROUPRECTANGLE_MEASUREMENTFRAMEGROUPRECTANGLEPLUGIN_H_
#define _SEIMPLEMENTATION_PLUGIN_MEASUREMENTFRAMEGROUPRECTANGLE_MEASUREMENTFRAMEGROUPRECTANGLEPLUGIN_H_

#include "SEFramework/Plugin/Plugin.h"

namespace SExtractor {

class MeasurementFrameGroupRectanglePlugin : public Plugin {

public:

  virtual ~MeasurementFrameGroupRectanglePlugin() = default;

  virtual void registerPlugin(PluginAPI& plugin_api) override;
  virtual std::string getIdString() const override;

private:

};

} // end SExtractor

#endif // _SEIMPLEMENTATION_PLUGIN_MEASUREMENTFRAMEGROUPRECTANGLE_MEASUREMENTFRAMEGROUPRECTANGLEPLUGIN_H_

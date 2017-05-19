/*
 * MeasurementFramePlugin.h
 *
 *  Created on: Nov 3, 2016
 *      Author: mschefer
 */

#ifndef SEIMPLEMENTATION_SEIMPLEMENTATION_PLUGIN_MEASUREMENTFRAME_MEASUREMENTFRAMEPLUGIN_H_
#define SEIMPLEMENTATION_SEIMPLEMENTATION_PLUGIN_MEASUREMENTFRAME_MEASUREMENTFRAMEPLUGIN_H_

#include "SEFramework/Plugin/Plugin.h"

namespace SExtractor {

class MeasurementFramePlugin : public Plugin {

public:

  /**
   * @brief Destructor
   */
  virtual ~MeasurementFramePlugin() = default;

  virtual void registerPlugin(PluginAPI& plugin_api) override;
  virtual std::string getIdString() const override;

private:

};

}

#endif

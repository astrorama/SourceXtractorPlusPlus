/*
 * PeakValuePlugin.h
 *
 *  Created on: Feb 9, 2017
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_PEAKVALUE_PEAKVALUEPLUGIN_H_
#define _SEIMPLEMENTATION_PLUGIN_PEAKVALUE_PEAKVALUEPLUGIN_H_

#include "SEFramework/Plugin/Plugin.h"

namespace SExtractor {

class PeakValuePlugin : public Plugin {

public:

  virtual ~PeakValuePlugin() = default;

  virtual void registerPlugin(PluginAPI& plugin_api) override;
  virtual std::string getIdString() const override;

private:

};

}


#endif /* _SEIMPLEMENTATION_PLUGIN_PEAKVALUE_PEAKVALUEPLUGIN_H_ */

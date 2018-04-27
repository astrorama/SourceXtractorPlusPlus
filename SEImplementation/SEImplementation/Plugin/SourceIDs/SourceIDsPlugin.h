/*
 * SourceIDsPlugin.h
 *
 *  Created on: Apr 27, 2018
 *      Author: mkuemmel@usm.lmu.de
 */

#ifndef __SEIMPLEMENTATION_PLUGIN_SOURCEIDS_SOURCEIDSPLUGIN_H_
#define __SEIMPLEMENTATION_PLUGIN_SOURCEIDS_SOURCEIDSPLUGIN_H_

#include "SEFramework/Plugin/Plugin.h"

namespace SExtractor {

class SourceIDsPlugin : public Plugin {

public:

  /**
   * @brief Destructor
   */
  virtual ~SourceIDsPlugin() = default;

  virtual void registerPlugin(PluginAPI& plugin_api) override;
  virtual std::string getIdString() const override;

private:

};

}


#endif /* __SEIMPLEMENTATION_PLUGIN_SOURCEIDS_SOURCEIDSPLUGIN_H_ */

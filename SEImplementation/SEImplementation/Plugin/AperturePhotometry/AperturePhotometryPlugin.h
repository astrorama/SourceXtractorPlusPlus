/*
 * AperturePhotometryPlugin.h
 *
 *  Created on: Sep 23, 2016
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_APERTUREPHOTOMETRY_APERTUREPHOTOMETRYPLUGIN_H_
#define _SEIMPLEMENTATION_PLUGIN_APERTUREPHOTOMETRY_APERTUREPHOTOMETRYPLUGIN_H_

#include "SEFramework/Plugin/Plugin.h"

namespace SExtractor {

class AperturePhotometryPlugin : public Plugin {

public:

  /**
   * @brief Destructor
   */
  virtual ~AperturePhotometryPlugin() = default;

  virtual void registerPlugin(PluginAPI& plugin_api) override;
  virtual std::string getIdString() const override;

private:

};

}


#endif /* _SEIMPLEMENTATION_PLUGIN_APERTUREPHOTOMETRY_APERTUREPHOTOMETRYPLUGIN_H_ */

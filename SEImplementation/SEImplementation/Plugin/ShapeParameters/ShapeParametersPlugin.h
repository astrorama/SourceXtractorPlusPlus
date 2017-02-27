/*
 * ShapeParametersPlugin.h
 *
 *  Created on: Jan 27, 2017
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_SHAPEPARAMETERS_SHAPEPARAMETERSPLUGIN_H_
#define _SEIMPLEMENTATION_PLUGIN_SHAPEPARAMETERS_SHAPEPARAMETERSPLUGIN_H_

#include "SEFramework/Plugin/Plugin.h"

namespace SExtractor {

class ShapeParametersPlugin : public Plugin {

public:

  /**
   * @brief Destructor
   */
  virtual ~ShapeParametersPlugin() = default;

  virtual void registerPlugin(PluginAPI& plugin_api) override;
  virtual std::string getIdString() const override;

private:

};

}




#endif /* _SEIMPLEMENTATION_PLUGIN_SHAPEPARAMETERS_SHAPEPARAMETERSPLUGIN_H_ */

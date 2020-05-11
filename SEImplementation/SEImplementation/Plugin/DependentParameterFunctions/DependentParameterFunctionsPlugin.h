/*
 * DependentParameterFunctionsPlugin.h
 *
 *  Created on: Apr 28, 2020
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_DEPENDENTPARAMETERFUNCTIONS_DEPENDENTPARAMETERFUNCTIONSPLUGIN_H_
#define _SEIMPLEMENTATION_PLUGIN_DEPENDENTPARAMETERFUNCTIONS_DEPENDENTPARAMETERFUNCTIONSPLUGIN_H_


#include "SEFramework/Plugin/Plugin.h"

namespace SourceXtractor {

class DependentParameterFunctionsPlugin : public Plugin {

public:

  /**
   * @brief Destructor
   */
  virtual ~DependentParameterFunctionsPlugin() = default;

  virtual void registerPlugin(PluginAPI& plugin_api) override;
  virtual std::string getIdString() const override;

private:

}; /* End of DependentParameterFunctionsPlugin class */

}

#endif /* _SEIMPLEMENTATION_PLUGIN_DEPENDENTPARAMETERFUNCTIONS_DEPENDENTPARAMETERFUNCTIONSPLUGIN_H_ */

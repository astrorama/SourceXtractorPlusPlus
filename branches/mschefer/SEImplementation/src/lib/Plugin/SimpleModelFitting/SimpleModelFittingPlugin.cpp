/*
 * SimpleModelFittingPlugin.cpp
 *
 *  Created on: May 2, 2017
 *      Author: mschefer
 */

#include "SEFramework/Plugin/StaticPlugin.h"

#include "SEImplementation/Plugin/SimpleModelFitting/SimpleModelFitting.h"
#include "SEImplementation/Plugin/SimpleModelFitting/SimpleModelFittingTaskFactory.h"

#include "SEImplementation/Plugin/SimpleModelFitting/SimpleModelFittingPlugin.h"

namespace SExtractor {

static StaticPlugin<SimpleModelFittingPlugin> shape_parameters_plugin;

void SimpleModelFittingPlugin::registerPlugin(PluginAPI& plugin_api) {
  plugin_api.getTaskFactoryRegistry().registerTaskFactory<SimpleModelFittingTaskFactory, SimpleModelFitting>();

  plugin_api.getOutputRegistry().registerColumnConverter<SimpleModelFitting, double>(
          "model_fitting_flux",
          [](const SimpleModelFitting& prop){
            return prop.getFlux();
          }
  );

  plugin_api.getOutputRegistry().registerColumnConverter<SimpleModelFitting, double>(
          "model_fitting_peak",
          [](const SimpleModelFitting& prop){
            return prop.getPeak();
          }
  );

  plugin_api.getOutputRegistry().registerColumnConverter<SimpleModelFitting, int>(
          "model_fitting_iterations",
          [](const SimpleModelFitting& prop){
            return prop.getIterations();
          }
  );

  plugin_api.getOutputRegistry().optionalOutput<SimpleModelFitting>("SimpleModelFitting");
}

std::string SimpleModelFittingPlugin::getIdString() const {
  return "SimpleModelFitting";
}

}

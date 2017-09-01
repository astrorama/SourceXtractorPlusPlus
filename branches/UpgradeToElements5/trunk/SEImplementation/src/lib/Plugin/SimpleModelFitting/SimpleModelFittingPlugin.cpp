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
          "model_fitting_x",
          [](const SimpleModelFitting& prop){
            return prop.getX();
          }
  );

  plugin_api.getOutputRegistry().registerColumnConverter<SimpleModelFitting, double>(
          "model_fitting_y",
          [](const SimpleModelFitting& prop){
            return prop.getY();
          }
  );

  plugin_api.getOutputRegistry().registerColumnConverter<SimpleModelFitting, double>(
          "model_fitting_wc_alpha",
          [](const SimpleModelFitting& prop){
            return prop.getAlpha();
          }
  );

  plugin_api.getOutputRegistry().registerColumnConverter<SimpleModelFitting, double>(
          "model_fitting_wc_delta",
          [](const SimpleModelFitting& prop){
            return prop.getDelta();
          }
  );

  plugin_api.getOutputRegistry().registerColumnConverter<SimpleModelFitting, double>(
          "model_fitting_exponential_ratio",
          [](const SimpleModelFitting& prop){
            return prop.getExponentialRatio();
          }
  );

  plugin_api.getOutputRegistry().registerColumnConverter<SimpleModelFitting, double>(
          "model_fitting_exponential_angle",
          [](const SimpleModelFitting& prop){
            return prop.getExponentialAngle() * 180.0 / M_PI;
          }
  );

  plugin_api.getOutputRegistry().registerColumnConverter<SimpleModelFitting, double>(
          "model_fitting_devaucouleurs_ratio",
          [](const SimpleModelFitting& prop){
            return prop.getDevaucouleursRatio();
          }
  );

  plugin_api.getOutputRegistry().registerColumnConverter<SimpleModelFitting, double>(
          "model_fitting_devaucouleurs_angle",
          [](const SimpleModelFitting& prop){
            return prop.getDevaucouleursAngle() * 180.0 / M_PI;
          }
  );

  plugin_api.getOutputRegistry().registerColumnConverter<SimpleModelFitting, double>(
          "model_fitting_flux",
          [](const SimpleModelFitting& prop){
            return prop.getTotalFlux();
          }
  );

//  plugin_api.getOutputRegistry().registerColumnConverter<SimpleModelFitting, double>(
//          "model_fitting_peak",
//          [](const SimpleModelFitting& prop){
//            return prop.getPeak();
//          }
//  );

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

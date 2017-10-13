/*
 * MultiframeModelFittingPlugin.cpp
 *
 *  Created on: Oct 5, 2017
 *      Author: mschefer
 */


#include "SEFramework/Plugin/StaticPlugin.h"

#include "SEImplementation/Plugin/MultiframeModelFitting/MultiframeModelFitting.h"
#include "SEImplementation/Plugin/MultiframeModelFitting/MultiframeModelFittingTaskFactory.h"

#include "SEImplementation/Plugin/MultiframeModelFitting/MultiframeModelFittingPlugin.h"

namespace SExtractor {

static StaticPlugin<MultiframeModelFittingPlugin> shape_parameters_plugin;

void MultiframeModelFittingPlugin::registerPlugin(PluginAPI& plugin_api) {
  plugin_api.getTaskFactoryRegistry().registerTaskFactory<MultiframeModelFittingTaskFactory, MultiframeModelFitting>();

  plugin_api.getOutputRegistry().registerColumnConverter<MultiframeModelFitting, double>(
          "model_fitting_x",
          [](const MultiframeModelFitting& prop){
            return prop.getX();
          }
  );

  plugin_api.getOutputRegistry().registerColumnConverter<MultiframeModelFitting, double>(
          "model_fitting_y",
          [](const MultiframeModelFitting& prop){
            return prop.getY();
          }
  );

//  plugin_api.getOutputRegistry().registerColumnConverter<MultiframeModelFitting, double>(
//          "model_fitting_wc_alpha",
//          [](const MultiframeModelFitting& prop){
//            return prop.getAlpha();
//          }
//  );
//
//  plugin_api.getOutputRegistry().registerColumnConverter<MultiframeModelFitting, double>(
//          "model_fitting_wc_delta",
//          [](const MultiframeModelFitting& prop){
//            return prop.getDelta();
//          }
//  );
//
//  plugin_api.getOutputRegistry().registerColumnConverter<MultiframeModelFitting, double>(
//          "model_fitting_exponential_ratio",
//          [](const MultiframeModelFitting& prop){
//            return prop.getExponentialRatio();
//          }
//  );
//
//  plugin_api.getOutputRegistry().registerColumnConverter<MultiframeModelFitting, double>(
//          "model_fitting_exponential_angle",
//          [](const MultiframeModelFitting& prop){
//            return prop.getExponentialAngle() * 180.0 / M_PI;
//          }
//  );
//
//  plugin_api.getOutputRegistry().registerColumnConverter<MultiframeModelFitting, double>(
//          "model_fitting_devaucouleurs_ratio",
//          [](const MultiframeModelFitting& prop){
//            return prop.getDevaucouleursRatio();
//          }
//  );
//
//  plugin_api.getOutputRegistry().registerColumnConverter<MultiframeModelFitting, double>(
//          "model_fitting_devaucouleurs_angle",
//          [](const MultiframeModelFitting& prop){
//            return prop.getDevaucouleursAngle() * 180.0 / M_PI;
//          }
//  );
//
//  plugin_api.getOutputRegistry().registerColumnConverter<MultiframeModelFitting, double>(
//          "model_fitting_flux",
//          [](const MultiframeModelFitting& prop){
//            return prop.getTotalFlux();
//          }
//  );
//
////  plugin_api.getOutputRegistry().registerColumnConverter<MultiframeModelFitting, double>(
////          "model_fitting_peak",
////          [](const MultiframeModelFitting& prop){
////            return prop.getPeak();
////          }
////  );

  plugin_api.getOutputRegistry().registerColumnConverter<MultiframeModelFitting, int>(
          "model_fitting_iterations",
          [](const MultiframeModelFitting& prop){
            return prop.getIterations();
          }
  );

  plugin_api.getOutputRegistry().optionalOutput<MultiframeModelFitting>("MultiframeModelFitting");
}

std::string MultiframeModelFittingPlugin::getIdString() const {
  return "MultiframeModelFitting";
}

}



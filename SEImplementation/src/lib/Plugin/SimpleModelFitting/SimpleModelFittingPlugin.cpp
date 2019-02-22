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

static StaticPlugin<SimpleModelFittingPlugin> simple_modelfitting_plugin;

void SimpleModelFittingPlugin::registerPlugin(PluginAPI& plugin_api) {
  plugin_api.getTaskFactoryRegistry().registerTaskFactory<SimpleModelFittingTaskFactory, SimpleModelFitting>();

  plugin_api.getOutputRegistry().registerColumnConverter<SimpleModelFitting, double>(
          "smf_x",
          [](const SimpleModelFitting& prop) {
            return prop.getX();
          }
  );

  plugin_api.getOutputRegistry().registerColumnConverter<SimpleModelFitting, double>(
          "smf_y",
          [](const SimpleModelFitting& prop) {
            return prop.getY();
          }
  );

  plugin_api.getOutputRegistry().registerColumnConverter<SimpleModelFitting, double>(
          "smf_alpha",
          [](const SimpleModelFitting& prop) {
            return prop.getAlpha();
          }
  );

  plugin_api.getOutputRegistry().registerColumnConverter<SimpleModelFitting, double>(
          "smf_delta",
          [](const SimpleModelFitting& prop) {
            return prop.getDelta();
          }
  );

  plugin_api.getOutputRegistry().registerColumnConverter<SimpleModelFitting, double>(
          "smf_exp_radius",
          [](const SimpleModelFitting& prop) {
            return prop.getExponentialRadius();
          }
  );

  plugin_api.getOutputRegistry().registerColumnConverter<SimpleModelFitting, double>(
          "smf_exp_ratio",
          [](const SimpleModelFitting& prop) {
            return prop.getExponentialRatio();
          }
  );

  plugin_api.getOutputRegistry().registerColumnConverter<SimpleModelFitting, double>(
          "smf_exp_angle",
          [](const SimpleModelFitting& prop) {
            return prop.getExponentialAngle() * 180.0 / M_PI;
          }
  );

  plugin_api.getOutputRegistry().registerColumnConverter<SimpleModelFitting, double>(
          "smf_dev_radius",
          [](const SimpleModelFitting& prop) {
            return prop.getDevaucouleursRadius();
          }
  );

  plugin_api.getOutputRegistry().registerColumnConverter<SimpleModelFitting, double>(
          "smf_dev_ratio",
          [](const SimpleModelFitting& prop) {
            return prop.getDevaucouleursRatio();
          }
  );

  plugin_api.getOutputRegistry().registerColumnConverter<SimpleModelFitting, double>(
          "smf_dev_angle",
          [](const SimpleModelFitting& prop) {
            return prop.getDevaucouleursAngle() * 180.0 / M_PI;
          }
  );

  plugin_api.getOutputRegistry().registerColumnConverter<SimpleModelFitting, double>(
          "smf_total_flux",
          [](const SimpleModelFitting& prop) {
            return prop.getTotalFlux();
          }
  );

  plugin_api.getOutputRegistry().registerColumnConverter<SimpleModelFitting, double>(
          "smf_exp_flux",
          [](const SimpleModelFitting& prop) {
            return prop.getExpFlux();
          }
  );

  plugin_api.getOutputRegistry().registerColumnConverter<SimpleModelFitting, double>(
          "smf_dev_flux",
          [](const SimpleModelFitting& prop) {
            return prop.getDevFlux();
          }
  );

  plugin_api.getOutputRegistry().registerColumnConverter<SimpleModelFitting, double>(
          "smf_bt_ratio",
          [](const SimpleModelFitting& prop) {
            return prop.getDevFlux() / (prop.getDevFlux() + prop.getExpFlux());
          }
  );

  plugin_api.getOutputRegistry().registerColumnConverter<SimpleModelFitting, int>(
          "smf_iter",
          [](const SimpleModelFitting& prop){
            return prop.getIterations();
          }
  );

  plugin_api.getOutputRegistry().enableOutput<SimpleModelFitting>("SimpleModelFitting");
}

std::string SimpleModelFittingPlugin::getIdString() const {
  return "SimpleModelFitting";
}

}

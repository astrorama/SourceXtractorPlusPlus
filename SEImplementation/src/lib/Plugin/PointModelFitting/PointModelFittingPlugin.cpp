/*
 * PointModelFittingPlugin.cpp
 *
 *  Created on: July 11, 2018
 *      Author: Alejandro Álvarez
 */

#include "SEFramework/Plugin/StaticPlugin.h"
#include "SEImplementation/Plugin/PointModelFitting/PointModelFittingTaskFactory.h"
#include "SEImplementation/Plugin/PointModelFitting/PointModelFittingPlugin.h"
#include "SEImplementation/Plugin/PointModelFitting/PointModelFitting.h"

namespace SExtractor {

static StaticPlugin<PointModelFittingPlugin> point_modelfitting_plugin;

std::string PointModelFittingPlugin::getIdString() const {
  return "PointModelFitting";
}

void PointModelFittingPlugin::registerPlugin(SExtractor::PluginAPI &plugin_api) {
  plugin_api.getTaskFactoryRegistry().registerTaskFactory<PointModelFittingTaskFactory, PointModelFitting>();

  plugin_api.getOutputRegistry().registerColumnConverter<PointModelFitting, SeFloat>(
    "pmf_x",
    [] (const PointModelFitting &model) {
      return model.getX();
    }
  );

  plugin_api.getOutputRegistry().registerColumnConverter<PointModelFitting, SeFloat>(
    "pmf_y",
    [] (const PointModelFitting &model) {
      return model.getY();
    }
  );

  plugin_api.getOutputRegistry().registerColumnConverter<PointModelFitting, SeFloat>(
    "pmf_alpha",
    [] (const PointModelFitting &model) {
      return model.getAlpha();
    }
  );

  plugin_api.getOutputRegistry().registerColumnConverter<PointModelFitting, SeFloat>(
    "pmf_delta",
    [] (const PointModelFitting &model) {
      return model.getDelta();
    }
  );

  plugin_api.getOutputRegistry().registerColumnConverter<PointModelFitting, SeFloat>(
    "pmf_total_flux",
    [] (const PointModelFitting &model) {
      return model.getTotalFlux();
    }
  );

  plugin_api.getOutputRegistry().registerColumnConverter<PointModelFitting, int>(
    "pmf_iter",
    [] (const PointModelFitting &model) {
      return model.getIterations();
    }
  );

  plugin_api.getOutputRegistry().enableOutput<PointModelFitting>("PointModelFitting");
}

} // SExtractor

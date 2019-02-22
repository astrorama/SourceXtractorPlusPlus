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

static StaticPlugin<MultiframeModelFittingPlugin> multiframe_modelfitting_plugin;

void MultiframeModelFittingPlugin::registerPlugin(PluginAPI& plugin_api) {
  plugin_api.getTaskFactoryRegistry().registerTaskFactory<MultiframeModelFittingTaskFactory, MultiframeModelFitting>();

  plugin_api.getOutputRegistry().registerColumnConverter<MultiframeModelFitting, double>(
          "mmf_x",
          [](const MultiframeModelFitting& prop) {
            return prop.getX();
          }
  );

  plugin_api.getOutputRegistry().registerColumnConverter<MultiframeModelFitting, double>(
          "mmf_y",
          [](const MultiframeModelFitting& prop){
            return prop.getY();
          }
  );

  plugin_api.getOutputRegistry().registerColumnConverter<MultiframeModelFitting, double>(
          "mmf_alpha",
          [](const MultiframeModelFitting& prop) {
            return prop.getAlpha();
          }
  );

  plugin_api.getOutputRegistry().registerColumnConverter<MultiframeModelFitting, double>(
          "mmf_delta",
          [](const MultiframeModelFitting& prop) {
            return prop.getDelta();
          }
  );

  plugin_api.getOutputRegistry().registerColumnConverter<MultiframeModelFitting, std::vector<double>>(
          "mmf_fluxes",
          [](const MultiframeModelFitting& prop) {
            return prop.getFluxes();
          }
  );

  plugin_api.getOutputRegistry().registerColumnConverter<MultiframeModelFitting, std::vector<double>>(
          "mmf_exp_fluxes",
          [](const MultiframeModelFitting& prop) {
            return prop.getExpFluxes();
          }
  );

  plugin_api.getOutputRegistry().registerColumnConverter<MultiframeModelFitting, std::vector<double>>(
          "mmf_dev_fluxes",
          [](const MultiframeModelFitting& prop) {
            return prop.getDevFluxes();
          }
  );

  plugin_api.getOutputRegistry().registerColumnConverter<MultiframeModelFitting, double>(
          "mmf_exp_radius",
          [](const MultiframeModelFitting& prop) {
            return prop.getExpRadius();
          }
  );

  plugin_api.getOutputRegistry().registerColumnConverter<MultiframeModelFitting, double>(
          "mmf_dev_radius",
          [](const MultiframeModelFitting& prop) {
            return prop.getDevRadius();
          }
  );

  plugin_api.getOutputRegistry().registerColumnConverter<MultiframeModelFitting, double>(
          "mmf_reduced_chi_2",
          [](const MultiframeModelFitting& prop) {
            return prop.getReducedChiSquared();
          }
  );

  plugin_api.getOutputRegistry().registerColumnConverter<MultiframeModelFitting, int>(
          "mmf_iterations",
          [](const MultiframeModelFitting& prop) {
            return prop.getIterations();
          }
  );
  plugin_api.getOutputRegistry().registerColumnConverter<MultiframeModelFitting, int>(
          "mmf_nframes",
          [](const MultiframeModelFitting& prop) {
            return prop.getFrameCount();
          }
  );

  plugin_api.getOutputRegistry().enableOutput<MultiframeModelFitting>("MultiframeModelFitting");
}

std::string MultiframeModelFittingPlugin::getIdString() const {
  return "MultiframeModelFitting";
}

}

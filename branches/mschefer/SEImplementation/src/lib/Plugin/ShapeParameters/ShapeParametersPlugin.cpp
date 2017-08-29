/*
 * ShapeParametersPlugin.cpp
 *
 *  Created on: Jan 27, 2017
 *      Author: mschefer
 */

#include "SEFramework/Plugin/StaticPlugin.h"

#include "SEImplementation/Plugin/ShapeParameters/ShapeParameters.h"
#include "SEImplementation/Plugin/ShapeParameters/ShapeParametersTaskFactory.h"

#include "SEImplementation/Plugin/ShapeParameters/ShapeParametersPlugin.h"

namespace SExtractor {

static StaticPlugin<ShapeParametersPlugin> shape_parameters_plugin;

void ShapeParametersPlugin::registerPlugin(PluginAPI& plugin_api) {
  plugin_api.getTaskFactoryRegistry().registerTaskFactory<ShapeParametersTaskFactory, ShapeParameters>();

  plugin_api.getOutputRegistry().registerColumnConverter<ShapeParameters, double>(
          "ellipse_a",
          [](const ShapeParameters& prop){
            return prop.getEllipseA();
          }
  );

  plugin_api.getOutputRegistry().registerColumnConverter<ShapeParameters, double>(
          "ellipse_b",
          [](const ShapeParameters& prop){
            return prop.getEllipseB();
          }
  );

  plugin_api.getOutputRegistry().registerColumnConverter<ShapeParameters, double>(
          "ellipse_theta",
          [](const ShapeParameters& prop){
            return prop.getEllipseTheta() * 180.0 / M_PI;
          }
  );

  plugin_api.getOutputRegistry().registerColumnConverter<ShapeParameters, double>(
          "ellipse_cxx",
          [](const ShapeParameters& prop){
            return prop.getEllipseCxx();
          }
  );

  plugin_api.getOutputRegistry().registerColumnConverter<ShapeParameters, double>(
          "ellipse_cyy",
          [](const ShapeParameters& prop){
            return prop.getEllipseCyy();
          }
  );

  plugin_api.getOutputRegistry().registerColumnConverter<ShapeParameters, double>(
          "ellipse_cxy",
          [](const ShapeParameters& prop){
            return prop.getEllipseCxy();
          }
  );

  plugin_api.getOutputRegistry().registerColumnConverter<ShapeParameters, double>(
          "abcor",
          [](const ShapeParameters& prop){
            return prop.getAbcor();
          }
  );

  plugin_api.getOutputRegistry().registerColumnConverter<ShapeParameters, double>(
          "area",
          [](const ShapeParameters& prop){
            return prop.getArea();
          }
  );

  plugin_api.getOutputRegistry().optionalOutput<ShapeParameters>("ShapeParameters");
}

std::string ShapeParametersPlugin::getIdString() const {
  return "ShapeParameters";
}

}


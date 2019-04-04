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
          },
          "[pixel]",
          "Profile RMS along major axis"
  );

  plugin_api.getOutputRegistry().registerColumnConverter<ShapeParameters, double>(
          "ellipse_b",
          [](const ShapeParameters& prop){
            return prop.getEllipseB();
          },
          "[pixel]",
          "Profile RMS along minor axis"
  );

  plugin_api.getOutputRegistry().registerColumnConverter<ShapeParameters, double>(
          "ellipse_theta",
          [](const ShapeParameters& prop){
            return prop.getEllipseTheta() * 180.0 / M_PI;
          },
          "[deg]",
          "Position angle (CCW/x)"
  );

  plugin_api.getOutputRegistry().registerColumnConverter<ShapeParameters, double>(
          "ellipse_cxx",
          [](const ShapeParameters& prop){
            return prop.getEllipseCxx();
          },
          "[pixel^{-2}]",
          "Cxx object ellipse parameter"
  );

  plugin_api.getOutputRegistry().registerColumnConverter<ShapeParameters, double>(
          "ellipse_cyy",
          [](const ShapeParameters& prop){
            return prop.getEllipseCyy();
          },
          "[pixel^{-2}]",
          "Cyy object ellipse parameter"
  );

  plugin_api.getOutputRegistry().registerColumnConverter<ShapeParameters, double>(
          "ellipse_cxy",
          [](const ShapeParameters& prop){
            return prop.getEllipseCxy();
          },
          "[pixel^{-2}]",
          "Cxy object ellipse parameter"
  );

  plugin_api.getOutputRegistry().registerColumnConverter<ShapeParameters, double>(
          "area",
          [](const ShapeParameters& prop){
            return prop.getArea();
          },
          "[]",
          "Total number of detected pixels"
  );

  plugin_api.getOutputRegistry().enableOutput<ShapeParameters>("ShapeParameters");
}

std::string ShapeParametersPlugin::getIdString() const {
  return "ShapeParameters";
}

}


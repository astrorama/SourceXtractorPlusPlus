/** Copyright © 2019 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3.0 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */
/*
 * ErrorEllipsePlugin.cpp
 *
 *  Created on: Feb 11 2022
 *      Author: mkuemmel
 */

#include "SEFramework/Plugin/StaticPlugin.h"

#include "SEImplementation/Plugin/ErrorEllipse/ErrorEllipse.h"
#include "SEImplementation/Plugin/ErrorEllipse/ErrorEllipseTaskFactory.h"

#include "SEImplementation/Plugin/ErrorEllipse/ErrorEllipsePlugin.h"

namespace SourceXtractor {

static StaticPlugin<ErrorEllipsePlugin> error_ellipse_plugin;

void ErrorEllipsePlugin::registerPlugin(PluginAPI& plugin_api) {
  plugin_api.getTaskFactoryRegistry().registerTaskFactory<ErrorEllipseTaskFactory, ErrorEllipse>();

  /*
  plugin_api.getOutputRegistry().registerColumnConverter<ErrorEllipse, double>(
          "ellipse_a_2",
          [](const ErrorEllipse& prop){
            return prop.getEllipseA();
          },
          "pixel",
          "Profile RMS along major axis"
  );

  plugin_api.getOutputRegistry().registerColumnConverter<ErrorEllipse, double>(
          "ellipse_b_2",
          [](const ErrorEllipse& prop){
            return prop.getEllipseB();
          },
          "pixel",
          "Profile RMS along minor axis"
  );

  plugin_api.getOutputRegistry().registerColumnConverter<ErrorEllipse, double>(
          "ellipse_theta_2",
          [](const ErrorEllipse& prop){
            return prop.getEllipseTheta() * 180.0 / M_PI;
          },
          "deg",
          "Position angle (CCW/x)"
  );
  */

  plugin_api.getOutputRegistry().registerColumnConverter<ErrorEllipse, double>(
          "ellipse_a_error_2",
          [](const ErrorEllipse& prop){
            return prop.getEllipseAError();
          },
          "pixel",
          "Error for profile RMS along major axis"
  );

  plugin_api.getOutputRegistry().registerColumnConverter<ErrorEllipse, double>(
          "ellipse_b_error_2",
          [](const ErrorEllipse& prop){
            return prop.getEllipseBError();
          },
          "pixel",
          "Error for profile RMS along minor axis"
  );

  plugin_api.getOutputRegistry().registerColumnConverter<ErrorEllipse, double>(
          "ellipse_theta_error_2",
          [](const ErrorEllipse& prop){
            return prop.getEllipseThetaError() * 180.0 / M_PI;
          },
          "deg",
          "Position angle error (CCW/x)"
  );

  /*
  plugin_api.getOutputRegistry().registerColumnConverter<ErrorEllipse, double>(
          "ellipse_cxx_2",
          [](const ErrorEllipse& prop){
            return prop.getEllipseCxx();
          },
          "pixel^{-2}",
          "Cxx object ellipse parameter"
  );

  plugin_api.getOutputRegistry().registerColumnConverter<ErrorEllipse, double>(
          "ellipse_cyy_2",
          [](const ErrorEllipse& prop){
            return prop.getEllipseCyy();
          },
          "pixel^{-2}",
          "Cyy object ellipse parameter"
  );

  plugin_api.getOutputRegistry().registerColumnConverter<ErrorEllipse, double>(
          "ellipse_cxy_2",
          [](const ErrorEllipse& prop){
            return prop.getEllipseCxy();
          },
          "pixel^{-2}",
          "Cxy object ellipse parameter"
  );
  */

  plugin_api.getOutputRegistry().registerColumnConverter<ErrorEllipse, double>(
          "ellipse_cxx_error_2",
          [](const ErrorEllipse& prop){
            return prop.getEllipseCxxError();
          },
          "pixel^{-2}",
          "Cxx object ellipse parameter error"
  );

  plugin_api.getOutputRegistry().registerColumnConverter<ErrorEllipse, double>(
          "ellipse_cyy_error_2",
          [](const ErrorEllipse& prop){
            return prop.getEllipseCyyError();
          },
          "pixel^{-2}",
          "Cyy object ellipse parameter error"
  );

  plugin_api.getOutputRegistry().registerColumnConverter<ErrorEllipse, double>(
          "ellipse_cxy_error_2",
          [](const ErrorEllipse& prop){
            return prop.getEllipseCxyError();
          },
          "pixel^{-2}",
          "Cxy object ellipse parameter error"
  );

  /*
  plugin_api.getOutputRegistry().registerColumnConverter<ErrorEllipse, double>(
          "area_2",
          [](const ErrorEllipse& prop){
            return prop.getArea();
          },
          "pixel",
          "Total number of detected pixels"
  );


  plugin_api.getOutputRegistry().registerColumnConverter<ErrorEllipse, double>(
          "elongation_2",
          [](const ErrorEllipse& prop) {
            return prop.getElongation();
          },
          "",
          "The object elongation (a_image / b_image)"
  );

  plugin_api.getOutputRegistry().registerColumnConverter<ErrorEllipse, double>(
          "ellipticity_2",
          [](const ErrorEllipse& prop) {
            return prop.getEllipticity();
          },
          "",
          "The object ellipticity"
  );
  */

  plugin_api.getOutputRegistry().enableOutput<ErrorEllipse>("ErrorEllipse");
}

std::string ErrorEllipsePlugin::getIdString() const {
  return "ErrorEllipse";
}

}


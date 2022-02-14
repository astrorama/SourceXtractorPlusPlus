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

  plugin_api.getOutputRegistry().registerColumnConverter<ErrorEllipse, double>(
          "error_ellipse_a",
          [](const ErrorEllipse& prop){
            return prop.getEllipseAError();
          },
          "pixel",
          "Major axis of the isophotal image centroid error ellipse"
  );

  plugin_api.getOutputRegistry().registerColumnConverter<ErrorEllipse, double>(
          "error_ellipse_b",
          [](const ErrorEllipse& prop){
            return prop.getEllipseBError();
          },
          "pixel",
          "Minor axis of the isophotal image centroid error ellipse"
  );

  plugin_api.getOutputRegistry().registerColumnConverter<ErrorEllipse, double>(
          "error_ellipse_theta",
          [](const ErrorEllipse& prop){
            return prop.getEllipseThetaError() * 180.0 / M_PI;
          },
          "deg",
          "Position angle of the isophotal image centroid ellipse"
  );

  plugin_api.getOutputRegistry().registerColumnConverter<ErrorEllipse, double>(
          "error_ellipse_cxx",
          [](const ErrorEllipse& prop){
            return prop.getEllipseCxxError();
          },
          "pixel^{-2}",
          "Isophotal image centroid Cxx error ellipse parameter"
  );

  plugin_api.getOutputRegistry().registerColumnConverter<ErrorEllipse, double>(
          "error_ellipse_cyy",
          [](const ErrorEllipse& prop){
            return prop.getEllipseCyyError();
          },
          "pixel^{-2}",
          "Isophotal image centroid Cyy error ellipse parameter"
  );

  plugin_api.getOutputRegistry().registerColumnConverter<ErrorEllipse, double>(
          "error_ellipse_cxy",
          [](const ErrorEllipse& prop){
            return prop.getEllipseCxyError();
          },
          "pixel^{-2}",
          "Isophotal image centroid Cxy error ellipse parameter"
  );

  plugin_api.getOutputRegistry().enableOutput<ErrorEllipse>("ErrorEllipse");
}

std::string ErrorEllipsePlugin::getIdString() const {
  return "ErrorEllipse";
}

}


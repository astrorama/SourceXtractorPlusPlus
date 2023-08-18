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
 * @file ObjectInfo.cpp
 * @author Nikolaos Apostolakos <nikoapos@gmail.com>
 */

#include <SEImplementation/PythonConfig/ObjectInfo.h>
#include <SEImplementation/Plugin/WorldCentroid/WorldCentroid.h>
#include <SEImplementation/Plugin/ReferenceCoordinates/ReferenceCoordinates.h>
#include <SEImplementation/Plugin/IsophotalFlux/IsophotalFlux.h>
#include <SEImplementation/Plugin/ShapeParameters/ShapeParameters.h>
#include <SEImplementation/Plugin/AssocMode/AssocMode.h>

namespace SourceXtractor {

ObjectInfo::ObjectInfo(const AssocModeConfig& config) {
  emplace(std::make_pair("centroid_x", 0.));
  emplace(std::make_pair("centroid_y", 0.));
  emplace(std::make_pair("isophotal_flux", 0.));
  emplace(std::make_pair("radius", 0.));
  emplace(std::make_pair("angle", 0.));
  emplace(std::make_pair("aspect_ratio", 0.));
  emplace(std::make_pair("assoc_match", false));
  emplace(std::make_pair("assoc_size", 0.));
  for (int i=0; i<100; i++) {
    std::stringstream label;
    label << "assoc_value_" << i;
    emplace(std::make_pair(label.str(), 0.));
  }
}

ObjectInfo::ObjectInfo(const SourceInterface& source, const AssocModeConfig& config) {
  auto world_centroid = source.getProperty<WorldCentroid>().getCentroid();
  auto reference_coordinates = source.getProperty<ReferenceCoordinates>().getCoordinateSystem();
  auto centroid = reference_coordinates->worldToImage(world_centroid);

  emplace(std::make_pair("centroid_x", centroid.m_x + 1.0));
  emplace(std::make_pair("centroid_y", centroid.m_y + 1.0));

  try {
    auto iso_flux = source.getProperty<IsophotalFlux>();
    emplace(std::make_pair("isophotal_flux", std::max<double>(iso_flux.getFlux(), 0.0001)));
  } catch (PropertyNotFoundException&) {
  }

  try {
    auto shape = source.getProperty<ShapeParameters>();
    double aspect_guess = std::max<double>(shape.getEllipseB() / shape.getEllipseA(), 0.01);

    emplace(std::make_pair("radius", std::max<double>(shape.getEllipseA() / 2.0, 0.01)));
    emplace(std::make_pair("angle", shape.getEllipseTheta()));
    emplace(std::make_pair("aspect_ratio", aspect_guess));
  } catch (PropertyNotFoundException&) {
  }

  auto assoc = source.getProperty<AssocMode>();
  emplace(std::make_pair("assoc_match", assoc.getMatch()));
  if (assoc.getMatch()) {
    emplace(std::make_pair("assoc_size", (double) assoc.getAssocValues().shape()[0]));
    int i=0;
    for (auto assoc_value : assoc.getAssocValues()) {
      std::stringstream label;
      label << "assoc_value_" << i;
      emplace(std::make_pair(label.str(), assoc_value));
      i++;
    }

  }
}

}

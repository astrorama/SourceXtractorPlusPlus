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
#include <SEImplementation/Plugin/PixelCentroid/PixelCentroid.h>
#include <SEImplementation/Plugin/IsophotalFlux/IsophotalFlux.h>
#include <SEImplementation/Plugin/ShapeParameters/ShapeParameters.h>

namespace SExtractor {

ObjectInfo::ObjectInfo(const SourceInterface& source) : m_source(source) { }

SeFloat ObjectInfo::getCentroidX() const {
  // On the Python side we use FITS coordinates, starting at 1
  return m_source.get().getProperty<PixelCentroid>().getCentroidX() + 1.0;
}

SeFloat ObjectInfo::getCentroidY() const {
  // On the Python side we use FITS coordinates, starting at 1
  return m_source.get().getProperty<PixelCentroid>().getCentroidY() + 1.0;
}

SeFloat ObjectInfo::getIsoFlux() const {
  return std::max<double>(m_source.get().getProperty<IsophotalFlux>().getFlux(), 0.0001);
}

SeFloat ObjectInfo::getRadius() const {
  return std::max<double>(m_source.get().getProperty<ShapeParameters>().getEllipseA() / 2.0, 0.01);
}

SeFloat ObjectInfo::getAngle() const {
  return -m_source.get().getProperty<ShapeParameters>().getEllipseTheta();
}

SeFloat ObjectInfo::getAspectRatio() const {
  auto& shape_parameters = m_source.get().getProperty<ShapeParameters>();
  double aspect_guess = std::max<double>(shape_parameters.getEllipseB() / shape_parameters.getEllipseA(), 0.01);

  return aspect_guess;
}

}

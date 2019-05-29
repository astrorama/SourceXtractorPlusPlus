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

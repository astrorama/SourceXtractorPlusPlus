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
  return m_source.get().getProperty<PixelCentroid>().getCentroidX();
}

SeFloat ObjectInfo::getCentroidY() const {
  return m_source.get().getProperty<PixelCentroid>().getCentroidY();
}

SeFloat ObjectInfo::getIsoFlux() const {
  return m_source.get().getProperty<IsophotalFlux>().getFlux();
}

SeFloat ObjectInfo::getRadius() const {
  return m_source.get().getProperty<ShapeParameters>().getEllipseA() / 2.0;
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

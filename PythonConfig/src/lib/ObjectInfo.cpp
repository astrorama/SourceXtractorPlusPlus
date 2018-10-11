/* 
 * @file ObjectInfo.cpp
 * @author Nikolaos Apostolakos <nikoapos@gmail.com>
 */

#include <PythonConfig/ObjectInfo.h>
#include <SEImplementation/Plugin/WorldCentroid/WorldCentroid.h>
#include <SEImplementation/Plugin/IsophotalFlux/IsophotalFlux.h>
#include <SEImplementation/Plugin/ShapeParameters/ShapeParameters.h>

namespace SExtractor {

ObjectInfo::ObjectInfo(SourceInterface& source) : m_source(source) { }

SeFloat ObjectInfo::getAlphaCoord() {
  return m_source.get().getProperty<WorldCentroid>().getCentroidAlpha();
}

SeFloat ObjectInfo::getDeltaCoord() {
  return m_source.get().getProperty<WorldCentroid>().getCentroidDelta();
}

SeFloat ObjectInfo::getIsoFlux() {
  return m_source.get().getProperty<IsophotalFlux>().getFlux();
}

SeFloat ObjectInfo::getRadiusWorld() {
  return m_source.get().getProperty<ShapeParameters>().getEllipseA() / 2.0;
}

}
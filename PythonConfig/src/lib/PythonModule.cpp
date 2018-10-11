/* 
 * @file PythonModule.cpp
 * @author Nikolaos Apostolakos <nikoapos@gmail.com>
 */

#include <boost/python.hpp>
#include <PythonConfig/ObjectInfo.h>

#include <SEFramework/Source/SimpleSourceFactory.h>
#include <SEImplementation/Plugin/WorldCentroid/WorldCentroid.h>
#include <SEImplementation/Plugin/IsophotalFlux/IsophotalFlux.h>
#include <SEImplementation/Plugin/ShapeParameters/ShapeParameters.h>
#include <iostream>



namespace bp = boost::python;

namespace SExtractor {

void tt(bp::object x) {
  if (x == bp::object()) {
    std::cout << "NULL\n";
    return;
  }
  
  SimpleSource source {};
  source.setProperty<WorldCentroid>(4.3, 5.6);
  source.setProperty<IsophotalFlux>(1,2,3,4);
  source.setProperty<ShapeParameters>(1,2,3,4,5,6,7,8);
  ObjectInfo o {source};
  std::cout << bp::extract<double>(x(o)) << std::endl;
}

BOOST_PYTHON_MODULE(libPythonConfig) {
  bp::class_<ObjectInfo>("ObjectInfo", bp::init<SourceInterface&>())
      .def("get_alpha_coord", &ObjectInfo::getAlphaCoord)
      .def("get_delta_coord", &ObjectInfo::getDeltaCoord)
      .def("get_iso_flux", &ObjectInfo::getIsoFlux)
      .def("get_radius_world", &ObjectInfo::getRadiusWorld);
  bp::def("tt", &tt);
}

} // namespace SExtractor
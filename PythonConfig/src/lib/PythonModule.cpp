/* 
 * @file PythonModule.cpp
 * @author Nikolaos Apostolakos <nikoapos@gmail.com>
 */

#include <boost/python.hpp>
#include <PythonConfig/ObjectInfo.h>
#include <PythonConfig/MeasurementImage.h>


namespace bp = boost::python;

namespace SExtractor {

BOOST_PYTHON_MODULE(libPythonConfig) {
  
  bp::class_<ObjectInfo>("ObjectInfo", bp::init<SourceInterface&>())
      .def("get_alpha_coord", &ObjectInfo::getAlphaCoord)
      .def("get_delta_coord", &ObjectInfo::getDeltaCoord)
      .def("get_iso_flux", &ObjectInfo::getIsoFlux)
      .def("get_radius_world", &ObjectInfo::getRadiusWorld);
  
  bp::class_<MeasurementImage>("MeasurementImage", bp::init<std::string, std::string, std::string>())
      .def_readonly("id", &MeasurementImage::id)
      .def_readonly("file", &MeasurementImage::file)
      .def_readonly("psf_file", &MeasurementImage::psf_file)
      .def_readonly("weight_file", &MeasurementImage::weight_file);
}

} // namespace SExtractor
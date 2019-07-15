/* 
 * @file PythonModule.cpp
 * @author Nikolaos Apostolakos <nikoapos@gmail.com>
 */

#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

#include <SEImplementation/PythonConfig/ObjectInfo.h>
#include <SEImplementation/PythonConfig/PyMeasurementImage.h>
#include <SEImplementation/PythonConfig/PyAperture.h>

#include <SEImplementation/PythonConfig/PythonModule.h>

namespace bp = boost::python;

namespace SExtractor {

BOOST_PYTHON_MODULE(_SExtractorPy) {
  
  bp::class_<ObjectInfo>("ObjectInfo",
    "A source detected by SExtractor after the segmentation and deblending", bp::init<SourceInterface&>())
      .def("get_centroid_x", &ObjectInfo::getCentroidX, "Get the X coordinate of the pixel centroid")
      .def("get_centroid_y", &ObjectInfo::getCentroidY, "Get the Y coordinate of the pixel centroid")
      .def("get_iso_flux", &ObjectInfo::getIsoFlux, "Get the isophotal flux")
      .def("get_radius", &ObjectInfo::getRadius, "Get the source semi-major axis, in pixels")
      .def("get_angle", &ObjectInfo::getAngle, "Get the source angle, in radians")
      .def("get_aspect_ratio", &ObjectInfo::getAspectRatio, "Get the aspect ratio");
  
  bp::class_<PyMeasurementImage>("MeasurementImage",
    "C++ part of the MeasurementImage", bp::init<std::string, std::string, std::string>())
      .def_readonly("id", &PyMeasurementImage::id)
      .def_readonly("file", &PyMeasurementImage::file)
      .def_readwrite("gain", &PyMeasurementImage::gain)
      .def_readwrite("saturation", &PyMeasurementImage::saturation)
      .def_readwrite("flux_scale", &PyMeasurementImage::flux_scale)
      .def_readonly("psf_file", &PyMeasurementImage::psf_file)
      .def_readonly("weight_file", &PyMeasurementImage::weight_file)
      .def_readwrite("weight_type", &PyMeasurementImage::weight_type)
      .def_readwrite("weight_absolute", &PyMeasurementImage::weight_absolute)
      .def_readwrite("weight_scaling", &PyMeasurementImage::weight_scaling)
      .def_readwrite("has_weight_threshold", &PyMeasurementImage::has_weight_threshold)
      .def_readwrite("weight_threshold", &PyMeasurementImage::weight_threshold);

  bp::class_<PyId>("Id", bp::init<>())
    .def_readonly("id", &PyId::id);

  bp::class_<PyAperture, bp::bases<PyId>>("Aperture",
    "Set of aperture photometries", bp::init<bp::list>())
    .def_readonly("apertures", &PyAperture::apertures, "List of aperture diameters, in pixels")
    .def("__str__", &PyAperture::toString)
    .def("__repr__", &PyAperture::toString);

  bp::class_<CoordinateSystem, boost::noncopyable>("CoordinateSystem",
    "Implements transformation of coordinates between image and world coordinates", bp::no_init)
      .def("image_to_world", &CoordinateSystem::imageToWorld)
      .def("world_to_image", &CoordinateSystem::worldToImage);
  bp::register_ptr_to_python<std::shared_ptr<CoordinateSystem>>();

  bp::class_<WorldCoordinate>("WorldCoordinate", "World coordinates")
      .def(bp::init<double, double>())
      .def_readwrite("alpha", &WorldCoordinate::m_alpha)
      .def_readwrite("delta", &WorldCoordinate::m_delta);

  bp::class_<ImageCoordinate>("ImageCoordinate", "Image coordinates, in pixels")
      .def(bp::init<double, double>())
      .def_readwrite("x", &ImageCoordinate::m_x)
      .def_readwrite("y", &ImageCoordinate::m_y);

  bp::class_<std::vector<double> >("_DoubleVector")
    .def(bp::vector_indexing_suite<std::vector<double> >());

  bp::class_<std::vector<float> >("_FloatVector")
    .def(bp::vector_indexing_suite<std::vector<float> >());
}

} // namespace SExtractor

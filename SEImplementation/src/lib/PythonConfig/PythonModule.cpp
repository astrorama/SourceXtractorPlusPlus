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
 * @file PythonModule.cpp
 * @author Nikolaos Apostolakos <nikoapos@gmail.com>
 */

#include <boost/python/class.hpp>
#include <boost/python/import.hpp>
#include <boost/python/to_python_converter.hpp>
#include <boost/python/tuple.hpp>
#include <boost/python/enum.hpp>
#include <boost/python/module.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/python/suite/indexing/map_indexing_suite.hpp>

#include <SEFramework/Source/SourceFlags.h>
#include <SEImplementation/PythonConfig/PyMeasurementImage.h>
#include <SEImplementation/PythonConfig/PyAperture.h>
#include <SEImplementation/PythonConfig/PythonModule.h>
#include <SEImplementation/PythonConfig/PyOutputWrapper.h>
#include <SEImplementation/PythonConfig/PyFitsFile.h>
#include <SEImplementation/PythonConfig/PyImageFile.h>

#include "SEFramework/Image/Dimensions.h"


namespace bp = boost::python;

namespace SourceXtractor {

namespace {

/// Converts a metadata value to its equivalent Python type
struct MetadataToPython : public boost::static_visitor<bp::object> {
  template <typename T>
  bp::object operator()(const T& value) const {
    return bp::object(value);
  }

  /// A single character (e.g. 'T') becomes a one-character str, not an int
  bp::object operator()(char value) const {
    return bp::object(std::string(1, value));
  }
};


/// Metadata::get() for Python, raising KeyError the way a mapping should
bp::object metadataGetItem(const Metadata& metadata, const std::string& key) {
  auto entry = metadata.get(key);

  if (!entry) {
    PyErr_SetString(PyExc_KeyError, key.c_str());
    bp::throw_error_already_set();
  }

  return boost::apply_visitor(MetadataToPython(), entry->m_value);
}


bool metadataContains(const Metadata& metadata, const std::string& key) {
  return metadata.contains(key);
}


/// An ImageSelector as its int, its str, or None
bp::object selectorToPython(const ImageSelector& selector) {
  if (selector.isIndex()) {
    return bp::object(selector.getIndex());
  }

  if (selector.isName()) {
    return bp::object(selector.getName());
  }

  return bp::object();
}


/// An int, a str or None as an ImageSelector
ImageSelector selectorFromPython(const bp::object& value) {
  if (value.is_none()) {
    return ImageSelector{};
  }

  bp::extract<int> index{value};
  if (index.check()) {
    return ImageSelector{index()};
  }

  bp::extract<std::string> name{value};
  if (name.check()) {
    return ImageSelector{name()};
  }

  PyErr_SetString(PyExc_TypeError,
                  "an image selector is an int, a str, or None");
  bp::throw_error_already_set();
  return ImageSelector{};
}


/// Hands a vector of strings to Python as a tuple, rather than a Boost wrapper
struct StringVectorToPython {
  static PyObject* convert(const std::vector<std::string>& strings) {
    bp::list items;
    for (const auto& string : strings) {
      items.append(string);
    }
    return bp::incref(bp::tuple(items).ptr());
  }
};


/// Hands Dimensions to Python as sourcextractor.dimensions.Dimensions
struct DimensionsToPython {
  static PyObject* convert(const Dimensions& dimensions) {
    // Imported here rather than at module initialisation: sourcextractor
    // imports from this module, which is not yet usable while it is being
    // built
    bp::object cls = bp::import("sourcextractor.dimensions").attr("Dimensions");
    bp::object order(std::string(dimensions.getAxisOrder() == AxisOrder::XY ? "xy" : "yx"));
    bp::object obj = cls(dimensions.getWidth(), dimensions.getHeight(),
                         dimensions.getDepth(), order);
    return bp::incref(obj.ptr());
  }
};

}  // anonymous namespace


BOOST_PYTHON_MODULE(_SourceXtractorPy) {

  bp::class_<PyOutputWrapper, boost::noncopyable>("OutputWrapper",
    "A file-like object used to wrap stdout and stderr", bp::no_init)
    .def_readonly("closed", &PyOutputWrapper::closed)
    .def("close", &PyOutputWrapper::close)
    .def("fileno", &PyOutputWrapper::fileno)
    .def("flush", &PyOutputWrapper::flush)
    .def("isatty", &PyOutputWrapper::isatty)
    .def("readable", &PyOutputWrapper::readable)
    .def("read", &PyOutputWrapper::read)
    .def("readline", &PyOutputWrapper::readline)
    .def("readlines", &PyOutputWrapper::readlines)
    .def("seek", &PyOutputWrapper::seek)
    .def("seekable", &PyOutputWrapper::seekable)
    .def("tell", &PyOutputWrapper::tell)
    .def("truncate", &PyOutputWrapper::truncate)
    .def("writable", &PyOutputWrapper::writable)
    .def("write", &PyOutputWrapper::write)
    .def("writelines", &PyOutputWrapper::writelines);

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
      .def_readwrite("weight_threshold", &PyMeasurementImage::weight_threshold)
      .def_readwrite("is_background_constant", &PyMeasurementImage::is_background_constant)
      .def_readwrite("constant_background_value", &PyMeasurementImage::constant_background_value)
      .def_readwrite("psf_hdu", &PyMeasurementImage::psf_hdu)
      .add_property("image",
          +[](const PyMeasurementImage& self) { return selectorToPython(self.image); },
          +[](PyMeasurementImage& self, const bp::object& value) {
            self.image = selectorFromPython(value);
          },
          "which image within the file: an index, a name, or None")
      .add_property("weight",
          +[](const PyMeasurementImage& self) { return selectorToPython(self.weight); },
          +[](PyMeasurementImage& self, const bp::object& value) {
            self.weight = selectorFromPython(value);
          },
          "which image within the weight file: an index, a name, or None")
      .def_readwrite("wcs", &PyMeasurementImage::wcs)
      .def_readwrite("is_data_cube", &PyMeasurementImage::is_data_cube)
      .def_readwrite("image_layer", &PyMeasurementImage::image_layer)
      .def_readwrite("weight_layer", &PyMeasurementImage::weight_layer)
      .def_readwrite("psf_renormalize", &PyMeasurementImage::psf_renormalize);
      ;

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

  bp::enum_<Flags>("Flags", "Source flags")
      .value("NONE", Flags::NONE)
      .value("BIASED", Flags::BIASED)
      .value("SATURATED", Flags::SATURATED)
      .value("BOUNDARY", Flags::BOUNDARY)
      .value("NEIGHBORS", Flags::NEIGHBORS)
      .value("OUTSIDE", Flags::OUTSIDE)
      .value("PARTIAL_FIT", Flags::PARTIAL_FIT)
      .value("INSUFFICIENT_DATA", Flags::INSUFFICIENT_DATA)
      .value("ERROR", Flags::ERROR);

  bp::class_<std::vector<int> >("_IntVector")
    .def(bp::vector_indexing_suite<std::vector<int> >());

  bp::class_<std::vector<unsigned int> >("_UIntVector")
    .def(bp::vector_indexing_suite<std::vector<unsigned int> >());

  bp::class_<std::map<std::string, std::string>>("_StringStringMap")
    .def(bp::map_indexing_suite<std::map<std::string, std::string>>());

  // Would be better if this implemented the full collections.abc.Mapping
  // interface, but undecided as yet what to do about that for ASDF trees.
  // Does that mean supporting nested collection types (lookups through
  // nested mappings/sequences), or flattening the entire ASDF tree (slow,
  // ugly)?  So for now this is just a containerlike interface with
  // __getitem__ and __contains__.
  bp::class_<Metadata, boost::noncopyable>("Metadata",
      "Keyed metadata read from an image file", bp::no_init)
    .def("__getitem__", &metadataGetItem)
    .def("__contains__", &metadataContains)
    .def("get", +[](const Metadata& m, const std::string& k, const bp::object& fallback) {
          return m.contains(k) ? metadataGetItem(m, k) : fallback;
        })
    .def("get", +[](const Metadata& m, const std::string& k) {
          return m.contains(k) ? metadataGetItem(m, k) : bp::object();
        });
  bp::register_ptr_to_python<std::shared_ptr<const Metadata>>();

  bp::to_python_converter<std::vector<std::string>, StringVectorToPython>();
  bp::to_python_converter<Dimensions, DimensionsToPython>();

  bp::class_<PyImageFile>("ImageFile", "An image file of any supported format, opened by "
      "SourceXtractor++", bp::init<const std::string&>())
    .def_readonly("filename", &PyImageFile::getFilename)
    .def_readonly("image_names", &PyImageFile::getImageNames,
                  "names of the images in the file, in file order")
    .def_readonly("format", &PyImageFile::getFormat,
                  "name of the file's format, e.g. \"fits\" or \"asdf\"")
    .def("get_metadata", &PyImageFile::getMetadata, "metadata of the named image")
    .def("get_metadata", &PyImageFile::getMetadataByIndex, "metadata of the image at this index")
    .def("get_dimensions", &PyImageFile::getDimensions, "dimensions of the named image")
    .def("get_dimensions", &PyImageFile::getDimensionsByIndex,
         "dimensions of the image at this index");

  bp::class_<PyFitsFile, bp::bases<PyImageFile>>("FitsFile",
      "A FITS file opened by SourceXtractor++", bp::init<const std::string&>())
    .def_readonly("image_hdus", &PyFitsFile::getImageHdus)
    .def("get_headers", &PyFitsFile::getHeaders, "get headers for hdu")
    .def("get_dimensions", &PyFitsFile::getDimensions, "get dimensions for hdu");

}

} // namespace SourceXtractor

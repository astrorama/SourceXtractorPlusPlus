/* 
 * @file PythonInterpreter.cpp
 * @author Nikolaos Apostolakos <nikoapos@gmail.com>
 */

#include <utility>
#include <boost/python.hpp>
#include <python2.7/Python.h>
#include <PythonConfig/PythonInterpreter.h>

namespace py = boost::python;

namespace SExtractor {

PythonInterpreter &PythonInterpreter::getSingleton() {
  static PythonInterpreter singleton{};
  return singleton;
}

PythonInterpreter::PythonInterpreter() {
  Py_Initialize();
}

void PythonInterpreter::runCode(const std::string &code) {
  py::object main_module = py::import("__main__");
  py::object main_namespace = main_module.attr("__dict__");
  py::exec(code.c_str(), main_namespace);
}

void PythonInterpreter::runFile(const std::string &filename) {
  py::object main_module = py::import("__main__");
  py::object main_namespace = main_module.attr("__dict__");
  py::exec_file(filename.c_str(), main_namespace);
}

std::map<int, PyMeasurementImage> PythonInterpreter::getMeasurementImages() {
  py::object meas_images_module = py::import("sextractorxx.config.measurement_images");
  py::dict images = py::extract<py::dict>(meas_images_module.attr("measurement_images"));
  py::list ids = images.keys();
  std::map<int, PyMeasurementImage> result{};
  for (int i = 0; i < py::len(ids); ++i) {
    int id = py::extract<int>(ids[i]);
    PyMeasurementImage im = py::extract<PyMeasurementImage>(images[ids[i]]);
    result.emplace(std::make_pair(id, im));
  }
  return result;
}

std::map<int, PyAperture> PythonInterpreter::getApertures() {
  py::object apertures_module = py::import("sextractorxx.config.aperture");
  py::dict apertures = py::extract<py::dict>(apertures_module.attr("apertures_for_image"));
  py::list ids = apertures.keys();
  std::map<int, PyAperture> result;
  for (int i = 0; i < py::len(ids); ++i) {
    int id = py::extract<int>(ids[i]);
    PyAperture ap = py::extract<PyAperture>(apertures[ids[i]]);
    result.emplace(std::make_pair(id, ap));
  }
  return result;
}

std::map<std::string, std::vector<int>> PythonInterpreter::getOutputColumns() {
  py::object output_module = py::import("sextractorxx.config.output");
  py::list output = py::extract<py::list>(output_module.attr("output_columns"));
  std::map<std::string, std::vector<int>> result;
  for (int i = 0; i < py::len(output); ++i) {
    py::tuple t = py::extract<py::tuple>(output[i]);
    std::string name = py::extract<std::string>(t[0]);
    auto extract_list = py::extract<py::list>(t[1]);

    if (extract_list.check()) {
      py::list cs = extract_list;
      for (int j = 0; j < py::len(cs); ++j) {
        int c = py::extract<int>(cs[j]);
        result[name].push_back(c);
      }
    }
    else {
      int c = py::extract<int>(t[1]);
      result[name].push_back(c);
    }
  }
  return result;
}

namespace {

std::map<int, boost::python::object> getParameters(const py::str& dict_name) {
  py::object model_fitting_module = py::import("sextractorxx.config.model_fitting");
  py::dict parameters = py::extract<py::dict>(model_fitting_module.attr(dict_name));
  py::list ids = parameters.keys();
  std::map<int, boost::python::object> result;
  for (int i = 0; i < py::len(ids); ++i) {
    int id = py::extract<int>(ids[i]);
    auto par = parameters[ids[i]];
    result.emplace(std::make_pair(id, par));
  }
  return result;
}

}

std::map<int, boost::python::object> PythonInterpreter::getConstantParameters() {
  return getParameters("constant_parameter_dict");
}

std::map<int, boost::python::object> PythonInterpreter::getFreeParameters() {
  return getParameters("free_parameter_dict");
}

}

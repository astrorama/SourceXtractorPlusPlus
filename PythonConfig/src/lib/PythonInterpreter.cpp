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

PythonInterpreter& PythonInterpreter::getSingleton() {
  static PythonInterpreter singleton {};
  return singleton;
}

PythonInterpreter::PythonInterpreter() {
  Py_Initialize();
}

PythonInterpreter::~PythonInterpreter() {
  Py_Finalize();
}

void PythonInterpreter::runCode(const std::string& code) {
  py::object main_module = py::import("__main__");
  py::object main_namespace = main_module.attr("__dict__");
  py::exec(code.c_str(), main_namespace);
}

void PythonInterpreter::runFile(const std::string& filename) {
  py::object main_module = py::import("__main__");
  py::object main_namespace = main_module.attr("__dict__");
  py::exec_file(filename.c_str(), main_namespace);
}

std::map<int, PyMeasurementImage> PythonInterpreter::getMeasurementImages() {
  py::object meas_images_module = py::import("sextractorxx.config.measurement_images");
  py::dict images = py::extract<py::dict>(meas_images_module.attr("measurement_images"));
  py::list ids = images.keys();
  std::map<int, PyMeasurementImage> result {};
  for (int i = 0; i < py::len(ids); ++i) {
    int id = py::extract<int>(ids[i]);
    PyMeasurementImage im = py::extract<PyMeasurementImage>(images[ids[i]]);
    result.emplace(std::make_pair(id, im));
  }
  return result;
}

}
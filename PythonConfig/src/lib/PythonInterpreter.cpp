/* 
 * @file PythonInterpreter.cpp
 * @author Nikolaos Apostolakos <nikoapos@gmail.com>
 */

#include <utility>
#include <boost/python.hpp>
#include <python2.7/Python.h>
#include <PythonConfig/PythonInterpreter.h>

namespace bp = boost::python;

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

void PythonInterpreter::runCode(const std::string& filename) {
  bp::object main_module = bp::import("__main__");
  bp::object main_namespace = main_module.attr("__dict__");
  bp::exec_file(filename.c_str(), main_namespace);
}

std::map<int, MeasurementImage> PythonInterpreter::getMeasurementImages() {
  bp::object meas_images_module = bp::import("sextractorxx.config.measurement_images");
  bp::dict images = bp::extract<bp::dict>(meas_images_module.attr("measurement_images"));
  bp::list ids = images.keys();
  std::map<int, MeasurementImage> result {};
  for (int i = 0; i < bp::len(ids); ++i) {
    int id = bp::extract<int>(ids[i]);
    MeasurementImage im = bp::extract<MeasurementImage>(images[ids[i]]);
    result.emplace(std::make_pair(id, im));
  }
  return result;
}

}
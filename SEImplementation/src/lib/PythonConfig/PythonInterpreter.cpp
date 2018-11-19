/* 
 * @file PythonInterpreter.cpp
 * @author Nikolaos Apostolakos <nikoapos@gmail.com>
 */

#include <utility>
#include <boost/python.hpp>
#include <python2.7/Python.h>
#include <SEImplementation/PythonConfig/PythonInterpreter.h>

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

std::map<std::string, std::vector<int>> PythonInterpreter::getModelFittingOutputColumns() {
  py::object output_module = py::import("sextractorxx.config.output");
  py::list output = py::extract<py::list>(output_module.attr("model_fitting_parameter_columns"));
  std::map<std::string, std::vector<int>> result;
  for (int i = 0; i < py::len(output); ++i) {
    py::tuple t = py::extract<py::tuple>(output[i]);
    std::string name = py::extract<std::string>(t[0]);
    auto extract_list = py::extract<py::list>(t[1]);

    if (extract_list.check()) {
      py::list cs = extract_list;
      for (int j = 0; j < py::len(cs); ++j) {
        int c = py::extract<int>(cs[j].attr("id"));
        result[name].push_back(c);
      }
    } else {
      int c = py::extract<int>(t[1]);
      result[name].push_back(c);
    }
  }
  return result;
}

namespace {

std::map<int, boost::python::object> getMapFromDict(const py::str &module_name, const py::str &dict_name) {
  py::object model_fitting_module = py::import(module_name);
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
  return getMapFromDict("sextractorxx.config.model_fitting", "constant_parameter_dict");
}

std::map<int, boost::python::object> PythonInterpreter::getFreeParameters() {
  return getMapFromDict("sextractorxx.config.model_fitting", "free_parameter_dict");
}

std::map<int, boost::python::object> PythonInterpreter::getDependentParameters() {
  return getMapFromDict("sextractorxx.config.model_fitting", "dependent_parameter_dict");
}

std::map<int, boost::python::object> PythonInterpreter::getPriors() {
  return getMapFromDict("sextractorxx.config.model_fitting", "prior_dict");
}

std::map<int, boost::python::object> PythonInterpreter::getPointSourceModels() {
  return getMapFromDict("sextractorxx.config.model_fitting", "point_source_model_dict");
}

std::map<int, boost::python::object> PythonInterpreter::getSersicModels() {
  return getMapFromDict("sextractorxx.config.model_fitting", "sersic_model_dict");
}

std::map<int, boost::python::object> PythonInterpreter::getExponentialModels() {
  return getMapFromDict("sextractorxx.config.model_fitting", "exponential_model_dict");
}

std::map<int, boost::python::object> PythonInterpreter::getDeVaucouleursModels() {
  return getMapFromDict("sextractorxx.config.model_fitting", "de_vaucouleurs_model_dict");
}

std::map<int, std::vector<int>> PythonInterpreter::getFrameModelsMap() {
  std::map<int, std::vector<int>> result{};
  py::object model_fitting_module = py::import("sextractorxx.config.model_fitting");
  py::dict frame_dict = py::extract<py::dict>(model_fitting_module.attr("frame_models_dict"));
  py::list frame_ids = frame_dict.keys();
  for (int i = 0; i < py::len(frame_ids); ++i) {
    int frame_id = py::extract<int>(frame_ids[i]);
    py::list model_ids = py::extract<py::list>(frame_dict[frame_ids[i]]);
    for (int j = 0; j < py::len(model_ids); ++j) {
      int model_id = py::extract<int>(model_ids[j]);
      result[frame_id].push_back(model_id);
    }
  }
  return result;
}

}

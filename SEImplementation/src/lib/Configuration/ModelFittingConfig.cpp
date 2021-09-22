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
 * @file ModelFittingConfig.cpp
 * @author Nikolaos Apostolakos <nikoapos@gmail.com>
 */

#include <string>
#include <boost/python/extract.hpp>
#include <boost/python/object.hpp>
#include <boost/python/tuple.hpp>
#include <boost/python/dict.hpp>

#include "ElementsKernel/Logging.h"

#include "ModelFitting/Engine/LeastSquareEngineManager.h"

#include "SEUtils/Python.h"

#include "SEImplementation/Plugin/FlexibleModelFitting/FlexibleModelFittingParameter.h"
#include "SEImplementation/Plugin/FlexibleModelFitting/FlexibleModelFittingConverterFactory.h"
#include "SEImplementation/PythonConfig/ObjectInfo.h"
#include "SEImplementation/Configuration/PythonConfig.h"
#include "SEImplementation/Common/OnnxModel.h"

#include "SEImplementation/Configuration/ModelFittingConfig.h"


using namespace Euclid::Configuration;
namespace py = boost::python;

static Elements::Logging logger = Elements::Logging::getLogger("Config");

namespace SourceXtractor {

/**
 * Wrap py::extract *and* the call so Python errors can be properly translated and logged
 * @tparam R
 *  Return type
 * @tparam T
 *  Variadic template for any arbitrary number of arguments
 * @param func
 *  Python function to be called
 * @param args
 *  Arguments for the Python function
 * @return
 *  Whatever the Python function returns
 * @throw
 *  Elements::Exception if either the call or the extract throw a Python exception
 */
template <typename R, typename ...T>
R py_call_wrapper(const py::object& func, T... args) {
  GILStateEnsure ensure;
  try {
    return py::extract<R>(func(args...));
  }
  catch (const py::error_already_set &e) {
    throw pyToElementsException(logger);
  }
}

/**
 * @brief Hold a reference to a Python object
 * @details
 *  A boost::python::object contains a pointer to the underlying Python struct, which is
 *  copied as-is (shared) when copied. When the boost::python::object is destroyed, it checks,
 *  and then decrements, the reference count. This destruction is *not* thread safe, as the pointer
 *  is not protected by a mutex or anything.
 *  This class holds a single reference to the Python object, and relies on the mechanism of
 *  std::shared_ptr to destroy the object once there is no one using it. std::shared_ptr *is*
 *  thread safe, unlike boost::python::object.
 */
class PyObjectHolder {
  public:
    PyObjectHolder(py::object&& obj): m_obj_ptr(std::make_shared<py::object>(obj)) {}

    PyObjectHolder(const PyObjectHolder&) = default;
    PyObjectHolder(PyObjectHolder&&) = default;

    operator const py::object&() const {
      return *m_obj_ptr;
    }

    const py::object& operator *() const {
      return *m_obj_ptr;
    }

    py::object attr(const char *name) {
      return m_obj_ptr->attr(name);
    }

  private:
    std::shared_ptr<py::object> m_obj_ptr;
};

ModelFittingConfig::ModelFittingConfig(long manager_id) : Configuration(manager_id) {
  declareDependency<PythonConfig>();
}

ModelFittingConfig::~ModelFittingConfig() {
  GILStateEnsure ensure;
  m_parameters.clear();
  m_models.clear();
  m_frames.clear();
  m_priors.clear();
  m_outputs.clear();
}

void ModelFittingConfig::initialize(const UserValues&) {
  GILStateEnsure ensure;
  try {
    initializeInner();
  }
  catch (py::error_already_set &e) {
    throw pyToElementsException(logger);
  }
}

void ModelFittingConfig::initializeInner() {
  for (auto& p : getDependency<PythonConfig>().getInterpreter().getConstantParameters()) {
    auto py_value_func = PyObjectHolder(p.second.attr("get_value")());
    auto value_func = [py_value_func] (const SourceInterface& o) -> double {
      ObjectInfo oi {o};
      return py_call_wrapper<double>(py_value_func, oi);
    };
    m_parameters[p.first] = std::make_shared<FlexibleModelFittingConstantParameter>(
                                                           p.first, value_func);
  }
  
  for (auto& p : getDependency<PythonConfig>().getInterpreter().getFreeParameters()) {
    auto py_init_value_func = PyObjectHolder(p.second.attr("get_init_value")());
    auto init_value_func = [py_init_value_func] (const SourceInterface& o) -> double {
      ObjectInfo oi {o};
      return py_call_wrapper<double>(py_init_value_func, oi);
    };

    auto py_range_obj = PyObjectHolder(p.second.attr("get_range")());

    std::shared_ptr<FlexibleModelFittingConverterFactory> converter;
    std::string type_string(py::extract<char const*>(py_range_obj.attr("__class__").attr("__name__")));
    if (type_string == "Unbounded") {
      auto py_factor_func = PyObjectHolder(py_range_obj.attr("get_normalization_factor")());
      auto factor_func = [py_factor_func] (double init, const SourceInterface& o) -> double {
        ObjectInfo oi {o};
        return py_call_wrapper<double>(py_factor_func, init, oi);
      };
      converter = std::make_shared<FlexibleModelFittingUnboundedConverterFactory>(factor_func);
    } else if (type_string == "Range") {
      auto py_range_func = PyObjectHolder(py_range_obj.attr("get_limits")());
      auto range_func = [py_range_func] (double init, const SourceInterface& o) -> std::pair<double, double> {
        ObjectInfo oi {o};
        py::tuple range = py_call_wrapper<py::tuple>(py_range_func, init, oi);
        double low = py::extract<double>(range[0]);
        double high = py::extract<double>(range[1]);
        return {low, high};
      };
      bool is_exponential = py::extract<int>(py_range_obj.attr("get_type")().attr("value")) == 2;

      if (is_exponential) {
        converter = std::make_shared<FlexibleModelFittingExponentialRangeConverterFactory>(range_func);
      } else {
        converter = std::make_shared<FlexibleModelFittingLinearRangeConverterFactory>(range_func);
      }
    } else {
      throw Elements::Exception("Unknown converter type: " + type_string);
    }
    m_parameters[p.first] = std::make_shared<FlexibleModelFittingFreeParameter>(
                          p.first, init_value_func, converter);
  }
  
  for (auto& p : getDependency<PythonConfig>().getInterpreter().getDependentParameters()) {
    auto py_func = PyObjectHolder(p.second.attr("func"));
    std::vector<std::shared_ptr<FlexibleModelFittingParameter>> params {};
    py::list param_ids = py::extract<py::list>(p.second.attr("params"));
    for (int i = 0; i < py::len(param_ids); ++i) {
      int id = py::extract<int>(param_ids[i]);
      params.push_back(m_parameters[id]);
    }

    auto dependent_func = [py_func](const std::shared_ptr<CoordinateSystem> &cs, const std::vector<double> &params) -> double {
      GILStateEnsure ensure;
      try {
        PythonInterpreter::getSingleton().setCoordinateSystem(cs);
        return py::extract<double>((*py_func)(*py::tuple(params)));
      }
      catch (const py::error_already_set&) {
        throw pyToElementsException(logger);
      }
    };

    m_parameters[p.first] = std::make_shared<FlexibleModelFittingDependentParameter>(
                                                      p.first, dependent_func, params);
  }
  
  for (auto& p : getDependency<PythonConfig>().getInterpreter().getConstantModels()) {
    int value_id = py::extract<int>(p.second.attr("value").attr("id"));
    m_models[p.first] = std::make_shared<FlexibleModelFittingConstantModel>(
        m_parameters[value_id]);
  }


  for (auto& p : getDependency<PythonConfig>().getInterpreter().getPointSourceModels()) {
    int x_coord_id = py::extract<int>(p.second.attr("x_coord").attr("id"));
    int y_coord_id = py::extract<int>(p.second.attr("y_coord").attr("id"));
    int flux_id = py::extract<int>(p.second.attr("flux").attr("id"));
    m_models[p.first] = std::make_shared<FlexibleModelFittingPointModel>(
        m_parameters[x_coord_id], m_parameters[y_coord_id], m_parameters[flux_id]);
  }
  
  for (auto& p : getDependency<PythonConfig>().getInterpreter().getSersicModels()) {
    int x_coord_id = py::extract<int>(p.second.attr("x_coord").attr("id"));
    int y_coord_id = py::extract<int>(p.second.attr("y_coord").attr("id"));
    int flux_id = py::extract<int>(p.second.attr("flux").attr("id"));
    int effective_radius_id = py::extract<int>(p.second.attr("effective_radius").attr("id"));
    int aspect_ratio_id = py::extract<int>(p.second.attr("aspect_ratio").attr("id"));
    int angle_id = py::extract<int>(p.second.attr("angle").attr("id"));
    int n_id = py::extract<int>(p.second.attr("n").attr("id"));
    m_models[p.first] = std::make_shared<FlexibleModelFittingSersicModel>(
        m_parameters[x_coord_id], m_parameters[y_coord_id], m_parameters[flux_id], m_parameters[n_id],
        m_parameters[effective_radius_id], m_parameters[aspect_ratio_id],
        m_parameters[angle_id]);
  }
  
  for (auto& p : getDependency<PythonConfig>().getInterpreter().getExponentialModels()) {
    int x_coord_id = py::extract<int>(p.second.attr("x_coord").attr("id"));
    int y_coord_id = py::extract<int>(p.second.attr("y_coord").attr("id"));
    int flux_id = py::extract<int>(p.second.attr("flux").attr("id"));
    int effective_radius_id = py::extract<int>(p.second.attr("effective_radius").attr("id"));
    int aspect_ratio_id = py::extract<int>(p.second.attr("aspect_ratio").attr("id"));
    int angle_id = py::extract<int>(p.second.attr("angle").attr("id"));
    m_models[p.first] = std::make_shared<FlexibleModelFittingExponentialModel>(
        m_parameters[x_coord_id], m_parameters[y_coord_id], m_parameters[flux_id],
        m_parameters[effective_radius_id], m_parameters[aspect_ratio_id], m_parameters[angle_id]);
  }
  
  for (auto& p : getDependency<PythonConfig>().getInterpreter().getDeVaucouleursModels()) {
    int x_coord_id = py::extract<int>(p.second.attr("x_coord").attr("id"));
    int y_coord_id = py::extract<int>(p.second.attr("y_coord").attr("id"));
    int flux_id = py::extract<int>(p.second.attr("flux").attr("id"));
    int effective_radius_id = py::extract<int>(p.second.attr("effective_radius").attr("id"));
    int aspect_ratio_id = py::extract<int>(p.second.attr("aspect_ratio").attr("id"));
    int angle_id = py::extract<int>(p.second.attr("angle").attr("id"));
    m_models[p.first] = std::make_shared<FlexibleModelFittingDevaucouleursModel>(
        m_parameters[x_coord_id], m_parameters[y_coord_id], m_parameters[flux_id],
        m_parameters[effective_radius_id], m_parameters[aspect_ratio_id], m_parameters[angle_id]);
  }
  
  for (auto& p : getDependency<PythonConfig>().getInterpreter().getOnnxModels()) {
    int x_coord_id = py::extract<int>(p.second.attr("x_coord").attr("id"));
    int y_coord_id = py::extract<int>(p.second.attr("y_coord").attr("id"));
    int flux_id = py::extract<int>(p.second.attr("flux").attr("id"));
    int aspect_ratio_id = py::extract<int>(p.second.attr("aspect_ratio").attr("id"));
    int angle_id = py::extract<int>(p.second.attr("angle").attr("id"));

    std::map<std::string, std::shared_ptr<FlexibleModelFittingParameter>> params;
    py::dict parameters = py::extract<py::dict>(p.second.attr("params"));
    py::list names = parameters.keys();
    for (int i = 0; i < py::len(names); ++i) {
      std::string name(py::extract<char const*>(names[i]));
      params[name] = m_parameters[py::extract<int>(parameters[names[i]].attr("id"))];
    }

    std::vector<std::shared_ptr<OnnxModel>> onnx_models;
    py::list models = py::extract<py::list>(p.second.attr("models"));
    for (int i = 0; i < py::len(models); ++i) {
      std::string model_filename(py::extract<char const*>(models[i]));
      std::cout << model_filename << "\n";
      onnx_models.emplace_back(std::make_shared<OnnxModel>(model_filename));
    }

    m_models[p.first] = std::make_shared<FlexibleModelFittingOnnxModel>(
        onnx_models, m_parameters[x_coord_id], m_parameters[y_coord_id], m_parameters[flux_id],
        m_parameters[aspect_ratio_id], m_parameters[angle_id], params);
  }

  for (auto& p : getDependency<PythonConfig>().getInterpreter().getFrameModelsMap()) {
    std::vector<std::shared_ptr<FlexibleModelFittingModel>> model_list {};
    for (int x : p.second) {
      model_list.push_back(m_models[x]);
    }
    m_frames.push_back(std::make_shared<FlexibleModelFittingFrame>(p.first, model_list));
  }
  
  for (auto& p : getDependency<PythonConfig>().getInterpreter().getPriors()) {
    auto& prior = p.second;
    int param_id = py::extract<int>(prior.attr("param"));
    auto param = m_parameters[param_id];
    auto py_value_func = PyObjectHolder(prior.attr("value"));
    auto value_func = [py_value_func] (const SourceInterface& o) -> double {
      ObjectInfo oi {o};
      return py_call_wrapper<double>(py_value_func, oi);
    };
    auto py_sigma_func = PyObjectHolder(prior.attr("sigma"));
    auto sigma_func = [py_sigma_func] (const SourceInterface& o) -> double {
      ObjectInfo oi {o};
      return py_call_wrapper<double>(py_sigma_func, oi);
    };
    m_priors[p.first] = std::make_shared<FlexibleModelFittingPrior>(param, value_func, sigma_func);
  }
  
  m_outputs = getDependency<PythonConfig>().getInterpreter().getModelFittingOutputColumns();

  auto parameters = getDependency<PythonConfig>().getInterpreter().getModelFittingParams();
  m_least_squares_engine = py::extract<std::string>(parameters["engine"]);
  if (m_least_squares_engine.empty()) {
    m_least_squares_engine = ModelFitting::LeastSquareEngineManager::getDefault();
  }
  m_max_iterations = py::extract<int>(parameters["max_iterations"]);
  m_modified_chi_squared_scale = py::extract<double>(parameters["modified_chi_squared_scale"]);
}

const std::map<int, std::shared_ptr<FlexibleModelFittingParameter>>& ModelFittingConfig::getParameters() const {
  return m_parameters;
}

const std::map<int, std::shared_ptr<FlexibleModelFittingModel>>& ModelFittingConfig::getModels() const {
  return m_models;
}

const std::vector<std::shared_ptr<FlexibleModelFittingFrame> >& ModelFittingConfig::getFrames() const {
  return m_frames;
}

const std::map<int, std::shared_ptr<FlexibleModelFittingPrior> >& ModelFittingConfig::getPriors() const {
  return m_priors;
}

const std::vector<std::pair<std::string, std::vector<int>>>& ModelFittingConfig::getOutputs() const {
  return m_outputs;
}

}

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

#include "ElementsKernel/Logging.h"
#include "ModelFitting/Engine/LeastSquareEngineManager.h"
#include "SEImplementation/Plugin/FlexibleModelFitting/FlexibleModelFittingParameter.h"
#include "SEImplementation/Plugin/FlexibleModelFitting/FlexibleModelFittingConverterFactory.h"
#include "SEImplementation/PythonConfig/ObjectInfo.h"
#include "SEImplementation/Configuration/PythonConfig.h"
#include "SEImplementation/Configuration/ModelFittingConfig.h"
#include "Pyston/GIL.h"
#include "Pyston/Exceptions.h"
#include "Pyston/ExpressionTreeBuilder.h"
#include "Pyston/Util/GraphvizGenerator.h"

#include <string>
#include <boost/python/extract.hpp>
#include <boost/python/object.hpp>

using namespace Euclid::Configuration;
namespace py = boost::python;
using Pyston::AttributeSet;

static Elements::Logging logger = Elements::Logging::getLogger("Config");

namespace SourceXtractor {

template<typename Signature>
struct FunctionFromPython {
};

template<>
struct FunctionFromPython<double(const SourceInterface&)> {

  static
  std::function<double(const SourceInterface&)> get(const char *readable,
                                                 Pyston::ExpressionTreeBuilder& builder,
                                                 py::object py_func) {
    auto wrapped = builder.build<double(const AttributeSet&)>(py_func, ObjectInfo{});

    if (!wrapped.isCompiled()) {
      logger.warn() << "Could not compile " << readable << ": " << wrapped.reason()->what();
      wrapped.reason()->log(log4cpp::Priority::DEBUG, logger);
    }
    else {
      logger.info() << readable << " compiled";
      Pyston::GraphvizGenerator gv(readable);
      wrapped.getTree()->visit(gv);
      logger.info() << gv.str();
    }

    return [wrapped](const SourceInterface& o) -> double {
      return wrapped(ObjectInfo{o});
    };
  }
};

template<>
struct FunctionFromPython<double(const Pyston::Context&, const std::vector<double>&)> {
  static
  std::function<double(const Pyston::Context&, const std::vector<double>&)>
    get(const char *readable, Pyston::ExpressionTreeBuilder& builder, py::object py_func,
        size_t nparams) {
    auto wrapped = builder.build<double(const std::vector<double>&)>(py_func, nparams);
    if (!wrapped.isCompiled()) {
      logger.warn() << "Could not compile " << readable << ": " << wrapped.reason()->what();
      wrapped.reason()->log(log4cpp::Priority::DEBUG, logger);
    }
    else {
      logger.info() << readable << " compiled";
      Pyston::GraphvizGenerator gv(readable);
      wrapped.getTree()->visit(gv);
      logger.info() << gv.str();
    }

    return wrapped;
  }
};

template<>
struct FunctionFromPython<double(double, const SourceInterface&)> {
  static
  std::function<double(double, const SourceInterface&)> get(const char *readable,
                                                            Pyston::ExpressionTreeBuilder& builder,
                                                            py::object py_func) {
    auto wrapped = builder.build<double(double, const AttributeSet&)>(py_func, ObjectInfo{});

    if (!wrapped.isCompiled()) {
      logger.warn() << "Could not compile " << readable << ": " << wrapped.reason()->what();
      wrapped.reason()->log(log4cpp::Priority::DEBUG, logger);
    }
    else {
      logger.info() << readable << " compiled";
      Pyston::GraphvizGenerator gv(readable);
      wrapped.getTree()->visit(gv);
      logger.info() << gv.str();
    }

    return [wrapped](double a, const SourceInterface& o) -> double {
      return wrapped(a, ObjectInfo{o});
    };
  }
};

ModelFittingConfig::ModelFittingConfig(long manager_id) : Configuration(manager_id) {
  declareDependency<PythonConfig>();
}

ModelFittingConfig::~ModelFittingConfig() {
  Pyston::GILLocker locker;
  m_parameters.clear();
  m_models.clear();
  m_frames.clear();
  m_priors.clear();
  m_outputs.clear();
}

void ModelFittingConfig::initialize(const UserValues&) {
  Pyston::GILLocker locker;
  try {
    initializeInner();
  }
  catch (py::error_already_set &e) {
    throw Pyston::Exception().log(log4cpp::Priority::ERROR, logger);
  }
}

static double image_to_world_alpha(const Pyston::Context& context, double x, double y) {
  auto coord_system = boost::any_cast<std::shared_ptr<CoordinateSystem>>(context.at("coordinate_system"));
  return coord_system->imageToWorld({x, y}).m_alpha;
}

static double image_to_world_delta(const Pyston::Context& context, double x, double y) {
  auto coord_system = boost::any_cast<std::shared_ptr<CoordinateSystem>>(context.at("coordinate_system"));
  return coord_system->imageToWorld({x, y}).m_delta;
}

void ModelFittingConfig::initializeInner() {
  Pyston::ExpressionTreeBuilder expr_builder;
  expr_builder.registerFunction<double(const Pyston::Context&, double, double)>(
    "image_to_world_alpha", image_to_world_alpha
  );
  expr_builder.registerFunction<double(const Pyston::Context&, double, double)>(
    "image_to_world_delta", image_to_world_delta
  );

  /* Constant parameters */
  for (auto& p : getDependency<PythonConfig>().getInterpreter().getConstantParameters()) {
    auto value_func = FunctionFromPython<double(const SourceInterface&)>::get(
      "Constant parameter", expr_builder, p.second.attr("get_value")
    );

    m_parameters[p.first] = std::make_shared<FlexibleModelFittingConstantParameter>(
                                                           p.first, value_func);
  }

  /* Free parameters */
  for (auto& p : getDependency<PythonConfig>().getInterpreter().getFreeParameters()) {
    auto init_value_func = FunctionFromPython<double(const SourceInterface&)>::get(
      "Free parameter", expr_builder, p.second.attr("get_init_value")
    );

    auto py_range_obj = p.second.attr("get_range")();

    std::shared_ptr<FlexibleModelFittingConverterFactory> converter;
    std::string type_string(py::extract<char const*>(py_range_obj.attr("__class__").attr("__name__")));

    if (type_string == "Unbounded") {
      auto factor_func = FunctionFromPython<double(double, const SourceInterface&)>::get(
        "Unbounded", expr_builder, py_range_obj.attr("get_normalization_factor")
      );
      converter = std::make_shared<FlexibleModelFittingUnboundedConverterFactory>(factor_func);
    } else if (type_string == "Range") {
      auto min_func = FunctionFromPython<double(double, const SourceInterface&)>::get(
        "Range min", expr_builder, py_range_obj.attr("get_min")
      );
      auto max_func = FunctionFromPython<double(double, const SourceInterface&)>::get(
        "Range max", expr_builder, py_range_obj.attr("get_max")
      );

      auto range_func = [min_func, max_func] (double init, const SourceInterface& o) -> std::pair<double, double> {
        return {min_func(init, o), max_func(init, o)};
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

  /* Dependent parameters */
  for (auto& p : getDependency<PythonConfig>().getInterpreter().getDependentParameters()) {
    auto py_func = p.second.attr("func");
    std::vector<std::shared_ptr<FlexibleModelFittingParameter>> params {};
    py::list param_ids = py::extract<py::list>(p.second.attr("params"));
    for (int i = 0; i < py::len(param_ids); ++i) {
      int id = py::extract<int>(param_ids[i]);
      params.push_back(m_parameters[id]);
    }

    auto dependent = FunctionFromPython<double(const Pyston::Context&, const std::vector<double>&)>
      ::get("Dependent parameter", expr_builder, py_func, params.size());

    auto dependent_func = [dependent](const std::shared_ptr<CoordinateSystem> &cs, const std::vector<double> &params) -> double {
      Pyston::Context context;
      context["coordinate_system"] = cs;
      return dependent(context, params);
    };

    m_parameters[p.first] = std::make_shared<FlexibleModelFittingDependentParameter>(
                                                      p.first, dependent_func, params);
  }

  for (auto& p : getDependency<PythonConfig>().getInterpreter().getConstantModels()) {
    int value_id = py::extract<int>(p.second.attr("value").attr("id"));
    m_models[p.first] = std::make_shared<FlexibleModelFittingConstantModel>(m_parameters[value_id]);
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

    auto value_func = FunctionFromPython<double(const SourceInterface&)>::get(
      "Prior mean", expr_builder, prior.attr("value")
    );
    auto sigma_func = FunctionFromPython<double(const SourceInterface&)>::get(
      "Prior sigma", expr_builder, prior.attr("sigma")
    );

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

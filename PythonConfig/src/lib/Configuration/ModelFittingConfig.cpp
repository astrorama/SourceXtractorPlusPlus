/* 
 * @file ModelFittingConfig.cpp
 * @author Nikolaos Apostolakos <nikoapos@gmail.com>
 */

#include <AlexandriaKernel/memory_tools.h>
#include <SEImplementation/Plugin/FlexibleModelFitting/FlexibleModelFittingParameter.h>
#include <PythonConfig/ObjectInfo.h>
#include <PythonConfig/Configuration/PythonConfig.h>
#include <PythonConfig/Configuration/ModelFittingConfig.h>

using namespace Euclid;
using namespace Euclid::Configuration;
namespace py = boost::python;

namespace SExtractor {

ModelFittingConfig::ModelFittingConfig(long manager_id) : Configuration(manager_id) {
  declareDependency<PythonConfig>();
}

void ModelFittingConfig::initialize(const UserValues&) {
  for (auto& p : getDependency<PythonConfig>().getInterpreter().getConstantParameters()) {
    py::object py_value_func = p.second.attr("get_value")();
    auto value_func = [py_value_func] (const SourceInterface& o) -> double {
      ObjectInfo oi {o};
      return py::extract<double>(py_value_func(oi));
    };
    m_parameters[p.first] = make_unique<FlexibleModelFittingConstantParameter>(value_func);
  }
  
  for (auto& p : getDependency<PythonConfig>().getInterpreter().getFreeParameters()) {
    py::object py_init_value_func = p.second.attr("get_init_value")();
    auto init_value_func = [py_init_value_func] (const SourceInterface& o) -> double {
      ObjectInfo oi {o};
      return py::extract<double>(py_init_value_func(oi));
    };
    py::object py_range_obj = p.second.attr("get_range")();
    py::object py_range_func = py_range_obj.attr("get_limits")();
    auto range_func = [py_range_func] (double init, const SourceInterface& o) -> std::pair<double, double> {
      ObjectInfo oi {o};
      py::tuple range = py::extract<py::tuple>(py_range_func(init, oi));
      double low = py::extract<double>(range[0]);
      double high = py::extract<double>(range[1]);
      return {low, high};
    };
    bool is_exponential = py::extract<int>(py_range_obj.attr("get_type")().attr("value")) == 2;
    m_parameters[p.first] = make_unique<FlexibleModelFittingFreeParameter>(init_value_func, range_func, is_exponential);
  }
}

const std::map<int, std::unique_ptr<FlexibleModelFittingParameter> >& ModelFittingConfig::getParameters() const {
  return m_parameters;
}

}
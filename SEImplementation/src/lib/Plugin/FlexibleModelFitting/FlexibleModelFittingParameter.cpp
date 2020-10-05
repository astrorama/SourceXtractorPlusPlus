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
 * FlexibleModelFittingParameter.cpp
 *
 *  Created on: Oct 8, 2018
 *      Author: mschefer
 */

#include <iostream>

#include <boost/version.hpp>
#if BOOST_VERSION >= 106700

#if BOOST_VERSION >= 107000
#include <boost/math/differentiation/finite_difference.hpp>
namespace bmd = boost::math::differentiation;
#else
#include <boost/math/tools/numerical_differentiation.hpp>
namespace bmd = boost::math::tools;
#endif

#endif

#include "AlexandriaKernel/memory_tools.h"
#include "ModelFitting/Parameters/ManualParameter.h"
#include "ModelFitting/Parameters/EngineParameter.h"
#include "ModelFitting/Parameters/DependentParameter.h"
#include "ModelFitting/Engine/EngineParameterManager.h"

#include "SEUtils/Python.h"
#include "SEUtils/NumericalDerivative.h"

#include "SEFramework/Source/SourceInterface.h"

#include "SEImplementation/Plugin/DetectionFrameCoordinates/DetectionFrameCoordinates.h"

#include "SEImplementation/Plugin/FlexibleModelFitting/FlexibleModelFittingParameter.h"
#include "SEImplementation/Plugin/FlexibleModelFitting/FlexibleModelFittingParameterManager.h"
#include "SEImplementation/Plugin/FlexibleModelFitting/FlexibleModelFittingConverterFactory.h"


static Elements::Logging logger = Elements::Logging::getLogger("ModelFitting");


namespace SourceXtractor {

using namespace ModelFitting;

FlexibleModelFittingParameter::FlexibleModelFittingParameter(int id) : m_id(id) { }

int FlexibleModelFittingParameter::getId() const {
  return m_id;
}

FlexibleModelFittingConstantParameter::FlexibleModelFittingConstantParameter(int id, ValueFunc value)
        : FlexibleModelFittingParameter(id), m_value(value) { }

std::shared_ptr<ModelFitting::BasicParameter> FlexibleModelFittingConstantParameter::create(
                                                            FlexibleModelFittingParameterManager& /*parameter_manager*/,
                                                            ModelFitting::EngineParameterManager& /*engine_manager*/,
                                                            const SourceInterface& source) const {
  return std::make_shared<ManualParameter>(m_value(source));
}

std::shared_ptr<ModelFitting::BasicParameter> FlexibleModelFittingFreeParameter::create(
                                                            FlexibleModelFittingParameterManager& /*parameter_manager*/,
                                                            ModelFitting::EngineParameterManager& engine_manager,
                                                            const SourceInterface& source) const {
  double initial_value = m_initial_value(source);

  auto converter = m_converter_factory->getConverter(initial_value, source);
  auto parameter = std::make_shared<EngineParameter>(initial_value, std::move(converter));
  engine_manager.registerParameter(parameter);

  return parameter;
}

double FlexibleModelFittingFreeParameter::getSigma(FlexibleModelFittingParameterManager& parameter_manager, const SourceInterface& source,
      const std::vector<double>& free_parameter_sigmas) const {
  auto modelfitting_parameter = parameter_manager.getParameter(source, shared_from_this());
  return free_parameter_sigmas[parameter_manager.getParameterIndex(source, shared_from_this())];
}


namespace {

template <typename T>
double doubleResolver(const T&) {
  return 0;
}


template<typename ... Parameters>
std::shared_ptr<ModelFitting::BasicParameter> createDependentParameterHelper(
                                       FlexibleModelFittingParameterManager& parameter_manager,
                                       const SourceInterface& source,
                                       FlexibleModelFittingDependentParameter::ValueFunc value_calculator,
                                       std::shared_ptr<Parameters>... parameters) {
  auto coordinate_system = source.getProperty<DetectionFrameCoordinates>().getCoordinateSystem();

  auto calc = [value_calculator, coordinate_system] (decltype(doubleResolver(std::declval<Parameters>()))... params) -> double {
    std::vector<double> materialized{params...};
    return value_calculator(coordinate_system, materialized);
  };
  return createDependentParameter(calc, parameter_manager.getParameter(source, parameters)...);
}

}


std::shared_ptr<ModelFitting::BasicParameter> FlexibleModelFittingDependentParameter::create(
                                                            FlexibleModelFittingParameterManager& parameter_manager,
                                                            ModelFitting::EngineParameterManager&,
                                                            const SourceInterface& source) const {
  switch (m_parameters.size()) {
  case 1:
    return createDependentParameterHelper(parameter_manager, source, m_value_calculator, 
        m_parameters[0]);
  case 2:
    return createDependentParameterHelper(parameter_manager, source, m_value_calculator, 
        m_parameters[0], m_parameters[1]);
  case 3:
    return createDependentParameterHelper(parameter_manager, source, m_value_calculator, 
        m_parameters[0], m_parameters[1], m_parameters[2]);
  case 4:
    return createDependentParameterHelper(parameter_manager, source, m_value_calculator, 
        m_parameters[0], m_parameters[1], m_parameters[2], m_parameters[3]);
  case 5:
    return createDependentParameterHelper(parameter_manager, source, m_value_calculator, 
        m_parameters[0], m_parameters[1], m_parameters[2], m_parameters[3], m_parameters[4]);
  case 6:
    return createDependentParameterHelper(parameter_manager, source, m_value_calculator, 
        m_parameters[0], m_parameters[1], m_parameters[2], m_parameters[3], m_parameters[4],
        m_parameters[5]);
  case 7:
    return createDependentParameterHelper(parameter_manager, source, m_value_calculator, 
        m_parameters[0], m_parameters[1], m_parameters[2], m_parameters[3], m_parameters[4],
        m_parameters[5], m_parameters[6]);
  case 8:
    return createDependentParameterHelper(parameter_manager, source, m_value_calculator, 
        m_parameters[0], m_parameters[1], m_parameters[2], m_parameters[3], m_parameters[4],
        m_parameters[5], m_parameters[6], m_parameters[7]);
  case 9:
    return createDependentParameterHelper(parameter_manager, source, m_value_calculator, 
        m_parameters[0], m_parameters[1], m_parameters[2], m_parameters[3], m_parameters[4],
        m_parameters[5], m_parameters[6], m_parameters[7], m_parameters[8]);
  case 10:
    return createDependentParameterHelper(parameter_manager, source, m_value_calculator, 
        m_parameters[0], m_parameters[1], m_parameters[2], m_parameters[3], m_parameters[4],
        m_parameters[5], m_parameters[6], m_parameters[7], m_parameters[8], m_parameters[9]);
  }
  throw Elements::Exception() << "Dependent parameters can depend on maximum 10 other parameters";
}

std::vector<double> FlexibleModelFittingDependentParameter::getPartialDerivatives(
    const SourceInterface& source, const std::vector<double>& param_values) const {
  assert(param_values.size() == m_parameters.size());

  std::vector<double> result(param_values.size());
  auto cs = source.getProperty<DetectionFrameCoordinates>().getCoordinateSystem();

  for (unsigned int i = 0; i < result.size(); i++) {

    auto f = [&](double x) {
        auto params = param_values;
        params[i] = x;
        return m_value_calculator(cs, params);
    };

#if BOOST_VERSION >= 106700
    result[i] = bmd::finite_difference_derivative(f, param_values[i]);
#else
    // if boost's function is unavailable use our own function
    result[i] = NumericalDerivative::centralDifference(f, param_values[i]);
#endif
  }

  return result;
}

double FlexibleModelFittingDependentParameter::getSigma(FlexibleModelFittingParameterManager& parameter_manager, const SourceInterface& source,
      const std::vector<double>& free_parameter_sigmas) const {
  auto dependees = getDependees();
  std::vector<double> values;

  for (auto& dependee : dependees) {
    values.emplace_back(parameter_manager.getParameter(source, dependee)->getValue());
  }

  double sigma = 0.0;
  auto partial_derivatives = getPartialDerivatives(source, values);

  assert(dependees.size() == partial_derivatives.size());
  for (unsigned int i = 0; i < partial_derivatives.size(); i++) {
    auto dependee_sigma = dependees[i]->getSigma(parameter_manager, source, free_parameter_sigmas);
    sigma += partial_derivatives[i] * partial_derivatives[i] * dependee_sigma * dependee_sigma;
  }
  sigma = sqrt(sigma);

  return sigma;
}

}

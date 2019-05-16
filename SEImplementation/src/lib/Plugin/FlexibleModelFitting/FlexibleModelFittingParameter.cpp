/*
 * FlexibleModelFittingParameter.cpp
 *
 *  Created on: Oct 8, 2018
 *      Author: mschefer
 */

#include <iostream>
#include <boost/python.hpp>

#include "ModelFitting/utils.h"

#include "ModelFitting/Parameters/ManualParameter.h"
#include "ModelFitting/Parameters/EngineParameter.h"
#include "ModelFitting/Parameters/DependentParameter.h"
#include "ModelFitting/Engine/EngineParameterManager.h"

#include "SEUtils/Python.h"

#include "SEFramework/Source/SourceInterface.h"

#include "SEImplementation/Plugin/FlexibleModelFitting/FlexibleModelFittingConverterFactory.h"
#include "SEImplementation/Plugin/FlexibleModelFitting/FlexibleModelFittingParameterManager.h"

#include "SEImplementation/Plugin/FlexibleModelFitting/FlexibleModelFittingParameter.h"

static Elements::Logging logger = Elements::Logging::getLogger("FlexibleModelFittingParameter");

namespace SExtractor {

using namespace ModelFitting;

std::mutex python_callback_mutex {};

FlexibleModelFittingParameter::FlexibleModelFittingParameter(int id) : m_id(id) { }

int FlexibleModelFittingParameter::getId() const {
  return m_id;
}

FlexibleModelFittingConstantParameter::FlexibleModelFittingConstantParameter(int id, ValueFunc value)
        : FlexibleModelFittingParameter(id), m_value(value) { }

std::shared_ptr<ModelFitting::BasicParameter> FlexibleModelFittingFreeParameter::create(
                                                            FlexibleModelFittingParameterManager& /*parameter_manager*/,
                                                            ModelFitting::EngineParameterManager& engine_manager,
                                                            const SourceInterface& source) const {
  std::lock_guard<std::mutex> guard {python_callback_mutex};
  double initial_value = m_initial_value(source);

  auto converter = m_converter_factory->getConverter(initial_value, source);
  auto parameter = std::make_shared<EngineParameter>(initial_value, std::move(converter));
  engine_manager.registerParameter(*parameter);

  return parameter;
}

std::shared_ptr<ModelFitting::BasicParameter> FlexibleModelFittingConstantParameter::create(
                                                            FlexibleModelFittingParameterManager& /*parameter_manager*/,
                                                            ModelFitting::EngineParameterManager& /*engine_manager*/,
                                                            const SourceInterface& source) const {
  std::lock_guard<std::mutex> guard {python_callback_mutex};
  return std::make_shared<ManualParameter>(m_value(source));
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
                                       boost::python::object value_calculator,
                                       std::shared_ptr<Parameters>... parameters) {
  auto calc = [value_calculator] (decltype(doubleResolver(std::declval<Parameters>()))... params) -> double {
    std::lock_guard<std::mutex> guard {python_callback_mutex};
    try {
      return boost::python::extract<double>(value_calculator(params...));
    }
    catch (const boost::python::error_already_set &e) {
      throw pyToElementsException(logger);
    }
  };
  return createDependentParameterPtr(calc, *(parameter_manager.getParameter(source, parameters))...);
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

}

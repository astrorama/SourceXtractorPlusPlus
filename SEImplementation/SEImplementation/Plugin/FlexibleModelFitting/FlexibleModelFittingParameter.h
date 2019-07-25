/*
 * FlexibleModelFittingParameter.h
 *
 *  Created on: Sep 17, 2018
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_FLEXIBLEMODELFITTING_FLEXIBLEMODELFITTINGPARAMETER_H_
#define _SEIMPLEMENTATION_PLUGIN_FLEXIBLEMODELFITTING_FLEXIBLEMODELFITTINGPARAMETER_H_

#include <memory>
#include <utility>
#include <functional>
#include <vector>
#include <mutex>
#include "SEFramework/CoordinateSystem/CoordinateSystem.h"

namespace ModelFitting {
  class BasicParameter;
  class EngineParameterManager;
}

namespace SExtractor {

extern std::recursive_mutex python_callback_mutex;

class SourceInterface;
class FlexibleModelFittingParameterManager;
class FlexibleModelFittingConverterFactory;

class FlexibleModelFittingParameter {
public:
  FlexibleModelFittingParameter(int id);
  virtual ~FlexibleModelFittingParameter() = default;

  virtual std::shared_ptr<ModelFitting::BasicParameter> create(
                                  FlexibleModelFittingParameterManager& parameter_manager,
                                  ModelFitting::EngineParameterManager& engine_manager,
                                  const SourceInterface& source) const = 0;
  
  virtual int getId() const final;

private:
  int m_id;
};

class FlexibleModelFittingConstantParameter : public FlexibleModelFittingParameter {
  
public:
  
  /// The signature of a function providing the parameters value. It gets as a
  /// parameter a source and returns the value.
  using ValueFunc = std::function<double(const SourceInterface&)>;
  
  FlexibleModelFittingConstantParameter(int id, ValueFunc value);

  std::shared_ptr<ModelFitting::BasicParameter> create(
                                  FlexibleModelFittingParameterManager& parameter_manager,
                                  ModelFitting::EngineParameterManager& engine_manager,
                                  const SourceInterface& source) const override;

private:

  ValueFunc m_value;
  
};

class FlexibleModelFittingFreeParameter : public FlexibleModelFittingParameter {
public:
  
  /// The signature of a function providing the initial value. It gets as a
  /// parameter a source and returns the initial value.
  using InitialValueFunc = std::function<double(const SourceInterface&)>;

  FlexibleModelFittingFreeParameter(int id, InitialValueFunc initial_value,
      std::shared_ptr<FlexibleModelFittingConverterFactory> converter_factory)
          : FlexibleModelFittingParameter(id),
            m_initial_value(initial_value),
            m_converter_factory(converter_factory) {}

  std::shared_ptr<ModelFitting::BasicParameter> create(
                                  FlexibleModelFittingParameterManager& parameter_manager,
                                  ModelFitting::EngineParameterManager& engine_manager,
                                  const SourceInterface& source) const override;

private:

  InitialValueFunc m_initial_value;
  std::shared_ptr<FlexibleModelFittingConverterFactory> m_converter_factory;
};

class FlexibleModelFittingDependentParameter : public FlexibleModelFittingParameter {
  
public:

  /// The signature of a function that evaluates the dependent parameter. It gets
  /// as parameters the values of those parameters on which this one depends.
  using ValueFunc = std::function<double(const std::shared_ptr<CoordinateSystem>&, const std::vector<double>&)>;
  
  FlexibleModelFittingDependentParameter(int id, ValueFunc value_calculator,
                                         std::vector<std::shared_ptr<FlexibleModelFittingParameter>> parameters)
          : FlexibleModelFittingParameter(id),
            m_value_calculator(value_calculator),
            m_parameters(parameters) { }

  std::shared_ptr<ModelFitting::BasicParameter> create(
                                  FlexibleModelFittingParameterManager& parameter_manager,
                                  ModelFitting::EngineParameterManager& engine_manager,
                                  const SourceInterface& source) const override;

  const std::vector<std::shared_ptr<FlexibleModelFittingParameter>>& getDependees() const {
    return m_parameters;
  }

private:

  ValueFunc m_value_calculator;
  std::vector<std::shared_ptr<FlexibleModelFittingParameter>> m_parameters;
  
};

}

#endif /* _SEIMPLEMENTATION_PLUGIN_FLEXIBLEMODELFITTING_FLEXIBLEMODELFITTINGPARAMETER_H_ */

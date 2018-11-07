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

namespace ModelFitting {
  class BasicParameter;
  class EngineParameterManager;
}

namespace SExtractor {

class SourceInterface;

class FlexibleModelFittingParameter {
public:
  FlexibleModelFittingParameter() {}
  virtual ~FlexibleModelFittingParameter() {}

  virtual std::shared_ptr<ModelFitting::BasicParameter> create(
                                  ModelFitting::EngineParameterManager& manager,
                                  const SourceInterface& source) const = 0;

private:
};

class FlexibleModelFittingConstantParameter : public FlexibleModelFittingParameter {
  
public:
  
  /// The signature of a function providing the parameters value. It gets as a
  /// parameter a source and returns the value.
  using ValueFunc = std::function<double(const SourceInterface&)>;
  
  FlexibleModelFittingConstantParameter(ValueFunc value);

  std::shared_ptr<ModelFitting::BasicParameter> create(
                                  ModelFitting::EngineParameterManager& manager,
                                  const SourceInterface& source) const override;

private:

  ValueFunc m_value;
  
};

class FlexibleModelFittingFreeParameter : public FlexibleModelFittingParameter {
  
public:
  
  /// The signature of a function providing the initial value. It gets as a
  /// parameter a source and returns the initial value.
  using InitialValueFunc = std::function<double(const SourceInterface&)>;
  /// The signature of a function providing the range of a parameter. It gets as
  /// as input the initial value of the parameter and the source, and it returns
  /// a pair containing the minimum and maximum values of the range.
  using RangeFunc = std::function<std::pair<double, double>(double, const SourceInterface&)>;

  FlexibleModelFittingFreeParameter(InitialValueFunc initial_value,
                                    RangeFunc range, bool is_exponential_range)
          : m_initial_value(initial_value),
            m_range(range),
            m_is_exponential_range(is_exponential_range) {}

  std::shared_ptr<ModelFitting::BasicParameter> create(
                                  ModelFitting::EngineParameterManager& manager,
                                  const SourceInterface& source) const override;

private:

  InitialValueFunc m_initial_value;
  RangeFunc m_range;
  bool m_is_exponential_range;
};

}

#endif /* _SEIMPLEMENTATION_PLUGIN_FLEXIBLEMODELFITTING_FLEXIBLEMODELFITTINGPARAMETER_H_ */

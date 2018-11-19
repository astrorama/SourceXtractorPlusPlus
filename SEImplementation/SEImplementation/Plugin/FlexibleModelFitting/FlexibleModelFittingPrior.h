/*
 * FlexibleModelFittingPrior.h
 *
 *  Created on: 2018 M11 16
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_FLEXIBLEMODELFITTING_FLEXIBLEMODELFITTINGPRIOR_H_
#define _SEIMPLEMENTATION_PLUGIN_FLEXIBLEMODELFITTING_FLEXIBLEMODELFITTINGPRIOR_H_

#include <functional>

#include "SEImplementation/Plugin/FlexibleModelFitting/FlexibleModelFittingParameter.h"

namespace ModelFitting {
  class ResidualEstimator;
}

namespace SExtractor {

class FlexibleModelFittingPrior {
public:
  using ValueFunc = std::function<double(const SourceInterface&)>;

  FlexibleModelFittingPrior(std::shared_ptr<FlexibleModelFittingParameter> parameter,
    ValueFunc expected_value, ValueFunc standard_deviation)
    : m_parameter(parameter), m_expected_value(expected_value), m_standard_deviation(standard_deviation) {}

  virtual ~FlexibleModelFittingPrior() {}

  void setupPrior(
      FlexibleModelFittingParameterManager& parameter_manager, const SourceInterface& source,
      ModelFitting::ResidualEstimator& residual_estimator) const;

private:
  std::shared_ptr<FlexibleModelFittingParameter> m_parameter;
  ValueFunc m_expected_value;
  ValueFunc m_standard_deviation;
};

}


#endif /* _SEIMPLEMENTATION_PLUGIN_FLEXIBLEMODELFITTING_FLEXIBLEMODELFITTINGPRIOR_H_ */

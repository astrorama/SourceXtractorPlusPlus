/*
 * FlexibleModelFittingParameter.h
 *
 *  Created on: Sep 17, 2018
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_FLEXIBLEMODELFITTING_FLEXIBLEMODELFITTINGPARAMETER_H_
#define _SEIMPLEMENTATION_PLUGIN_FLEXIBLEMODELFITTING_FLEXIBLEMODELFITTINGPARAMETER_H_

#include <memory>

namespace ModelFitting {
  class BasicParameter;
  class EngineParameterManager;
}

namespace SExtractor {

class FlexibleModelFittingParameter {
public:
  FlexibleModelFittingParameter() {}
  virtual ~FlexibleModelFittingParameter() {}

  virtual std::shared_ptr<ModelFitting::BasicParameter> create(ModelFitting::EngineParameterManager& manager) const = 0;

private:
};

class FlexibleModelFittingFreeParameter : public FlexibleModelFittingParameter {

  FlexibleModelFittingFreeParameter(double initial_value,
          double minimum_value, double maximum_value, bool is_exponential_range)
          : m_initial_value(initial_value),
            m_minimum_value(minimum_value),
            m_maximum_value(maximum_value),
            m_is_exponential_range(is_exponential_range) {}

private:
  // FIXME this will be replaced by Python callbacks
  double m_initial_value;
  double m_minimum_value;
  double m_maximum_value;
  bool m_is_exponential_range;

  virtual std::shared_ptr<ModelFitting::BasicParameter> create(ModelFitting::EngineParameterManager& manager) const;
};

class FlexibleModelFittingConstantParameter : public FlexibleModelFittingParameter {

  FlexibleModelFittingConstantParameter(double value)
          : m_value(value) {}

private:
  double m_value;

  virtual std::shared_ptr<ModelFitting::BasicParameter> create(ModelFitting::EngineParameterManager& manager) const;
};

}

#endif /* _SEIMPLEMENTATION_PLUGIN_FLEXIBLEMODELFITTING_FLEXIBLEMODELFITTINGPARAMETER_H_ */

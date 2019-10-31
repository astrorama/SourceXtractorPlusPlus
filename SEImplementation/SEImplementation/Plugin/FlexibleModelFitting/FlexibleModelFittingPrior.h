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

namespace SourceXtractor {

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

/*
 * FlexibleModelFittingConverterFactory.cpp
 *
 *  Created on: May 10, 2019
 *      Author: mschefer
 */

#include "ModelFitting/Parameters/ExpSigmoidConverter.h"
#include "ModelFitting/Parameters/SigmoidConverter.h"
#include "ModelFitting/Parameters/NormalizedConverter.h"
#include "ModelFitting/utils.h"

#include "SEImplementation/Plugin/FlexibleModelFitting/FlexibleModelFittingConverterFactory.h"
#include "SEUtils/Python.h"

namespace SExtractor {

using namespace ModelFitting;

std::unique_ptr<CoordinateConverter> FlexibleModelFittingExponentialRangeConverterFactory::getConverter(
    double initial_value, const SourceInterface& source) const {
  GILStateEnsure ensure;

  double minimum_value, maximum_value;
  std::tie(minimum_value, maximum_value) = m_range(initial_value, source);
  return make_unique<ExpSigmoidConverter>(minimum_value, maximum_value);
}

std::unique_ptr<CoordinateConverter> FlexibleModelFittingLinearRangeConverterFactory::getConverter(
    double initial_value, const SourceInterface& source) const {
  GILStateEnsure ensure;

  double minimum_value, maximum_value;
  std::tie(minimum_value, maximum_value) = m_range(initial_value, source);
  return make_unique<SigmoidConverter>(minimum_value, maximum_value);
}

std::unique_ptr<ModelFitting::CoordinateConverter> FlexibleModelFittingUnboundedConverterFactory::getConverter(
    double initial_value, const SourceInterface& source) const {
  GILStateEnsure ensure;

  double factor = m_normalization_factor(initial_value, source);
  return make_unique<NormalizedConverter>(factor);
}


}


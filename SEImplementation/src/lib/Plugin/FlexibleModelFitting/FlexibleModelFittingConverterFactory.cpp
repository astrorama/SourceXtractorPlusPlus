/*
 * FlexibleModelFittingConverterFactory.cpp
 *
 *  Created on: May 10, 2019
 *      Author: mschefer
 */

#include "ModelFitting/Parameters/ExpSigmoidConverter.h"
#include "ModelFitting/Parameters/SigmoidConverter.h"
#include "ModelFitting/utils.h"

#include "SEImplementation/Plugin/FlexibleModelFitting/FlexibleModelFittingConverterFactory.h"

namespace SExtractor {

using namespace ModelFitting;

std::unique_ptr<CoordinateConverter> FlexibleModelFittingExponentialRangeConverterFactory::getConverter(
    double initial_value, const SourceInterface& source) const {

  double minimum_value, maximum_value;
  std::tie(minimum_value, maximum_value) = m_range(initial_value, source);
  auto converter = make_unique<ExpSigmoidConverter>(minimum_value, maximum_value);
  return converter;
}

std::unique_ptr<CoordinateConverter> FlexibleModelFittingLinearRangeConverterFactory::getConverter(
    double initial_value, const SourceInterface& source) const {

  double minimum_value, maximum_value;
  std::tie(minimum_value, maximum_value) = m_range(initial_value, source);
  auto converter = make_unique<SigmoidConverter>(minimum_value, maximum_value);
  return converter;
}

}


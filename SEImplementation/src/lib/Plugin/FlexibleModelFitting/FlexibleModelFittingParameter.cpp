/*
 * FlexibleModelFittingParameter.cpp
 *
 *  Created on: Oct 8, 2018
 *      Author: mschefer
 */

#include "ModelFitting/utils.h"

#include "ModelFitting/Parameters/ManualParameter.h"
#include "ModelFitting/Parameters/EngineParameter.h"
#include "ModelFitting/Parameters/DependentParameter.h"
#include "ModelFitting/Engine/EngineParameterManager.h"

#include "ModelFitting/Parameters/ExpSigmoidConverter.h"
#include "ModelFitting/Parameters/SigmoidConverter.h"

#include "SEFramework/Source/SourceInterface.h"
#include "SEImplementation/Plugin/FlexibleModelFitting/FlexibleModelFittingParameter.h"

namespace SExtractor {

using namespace ModelFitting;

FlexibleModelFittingConstantParameter::FlexibleModelFittingConstantParameter(ValueFunc value)
        : m_value(value) { }

std::shared_ptr<ModelFitting::BasicParameter> FlexibleModelFittingConstantParameter::create(
                                      ModelFitting::EngineParameterManager&,
                                      const SourceInterface& source) const {
  double value = m_value(source);
  auto parameter = std::make_shared<ManualParameter>(value);
  return parameter;
}

std::shared_ptr<ModelFitting::BasicParameter> FlexibleModelFittingFreeParameter::create(
                                                            ModelFitting::EngineParameterManager& manager,
                                                            const SourceInterface& source) const {
  double initial_value = m_initial_value(source);
  double minimum_value, maximum_value;
  std::tie(minimum_value, maximum_value) = m_range(initial_value, source);
  
  std::unique_ptr<CoordinateConverter> converter;
  if (m_is_exponential_range)
    converter = make_unique<ExpSigmoidConverter>(minimum_value, maximum_value);
  else
    converter = make_unique<SigmoidConverter>(minimum_value, maximum_value);
  
  auto parameter = std::make_shared<EngineParameter>(initial_value, std::move(converter));
  manager.registerParameter(*parameter);
  return parameter;
}

std::shared_ptr<ModelFitting::BasicParameter> FlexibleModelFittingConstantParameter::create(
                                                            ModelFitting::EngineParameterManager& manager) const {
    return std::make_shared<ManualParameter>(m_value);
}


}

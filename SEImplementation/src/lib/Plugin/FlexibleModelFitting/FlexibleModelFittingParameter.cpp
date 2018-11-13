/*
 * FlexibleModelFittingParameter.cpp
 *
 *  Created on: Oct 8, 2018
 *      Author: mschefer
 */

#include <iostream>

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

std::shared_ptr<ModelFitting::BasicParameter> FlexibleModelFittingFreeParameter::create(
                                                            FlexibleModelFittingParameterManager& /*parameter_manager*/,
                                                            ModelFitting::EngineParameterManager& engine_manager,
                                                            const SourceInterface& source) const {
  double initial_value = m_initial_value(source);

  double minimum_value, maximum_value;
  std::tie(minimum_value, maximum_value) = m_range(initial_value, source);
  
  //std::cout << initial_value << " " << minimum_value << " " << maximum_value << "\n";

  std::unique_ptr<CoordinateConverter> converter;
  if (m_is_exponential_range) {
    converter = make_unique<ExpSigmoidConverter>(minimum_value, maximum_value);
  } else {
    converter = make_unique<SigmoidConverter>(minimum_value, maximum_value);
  }
  
  auto parameter = std::make_shared<EngineParameter>(initial_value, std::move(converter));
  engine_manager.registerParameter(*parameter);
  return parameter;
}

std::shared_ptr<ModelFitting::BasicParameter> FlexibleModelFittingConstantParameter::create(
                                                            FlexibleModelFittingParameterManager& /*parameter_manager*/,
                                                            ModelFitting::EngineParameterManager& /*engine_manager*/,
                                                            const SourceInterface& source) const {
    return std::make_shared<ManualParameter>(m_value(source));
}


}

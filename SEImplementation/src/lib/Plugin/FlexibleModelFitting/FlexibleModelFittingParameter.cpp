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

#include "SEImplementation/Plugin/FlexibleModelFitting/FlexibleModelFittingParameter.h"

namespace SExtractor {

using namespace ModelFitting;

std::shared_ptr<ModelFitting::BasicParameter> FlexibleModelFittingFreeParameter::create(
                                                            ModelFitting::EngineParameterManager& manager) const {
  std::shared_ptr<EngineParameter> parameter;
  if (m_is_exponential_range) {
    parameter = std::make_shared<EngineParameter>(m_initial_value, make_unique<ExpSigmoidConverter>(m_minimum_value, m_maximum_value));
  } else {
    parameter = std::make_shared<EngineParameter>(m_initial_value, make_unique<SigmoidConverter>(m_minimum_value, m_maximum_value));
  }

  manager.registerParameter(*parameter);
  return parameter;
}

std::shared_ptr<ModelFitting::BasicParameter> FlexibleModelFittingConstantParameter::create(
                                                            ModelFitting::EngineParameterManager& manager) const {
    return std::make_shared<ManualParameter>(m_value);
}


}

/*
 * FlexibleModelFittingPrior.cpp
 *
 *  Created on: 2018 M11 16
 *      Author: mschefer
 */

#include <iostream>
#include <mutex>

#include "ModelFitting/utils.h"

#include "ModelFitting/Parameters/ManualParameter.h"
#include "ModelFitting/Parameters/EngineParameter.h"
#include "ModelFitting/Parameters/DependentParameter.h"
#include "ModelFitting/Engine/EngineParameterManager.h"


#include "ModelFitting/Engine/WorldValueResidual.h"
#include "ModelFitting/Engine/ResidualEstimator.h"

#include "SEFramework/Source/SourceInterface.h"
#include "SEImplementation/Plugin/FlexibleModelFitting/FlexibleModelFittingParameter.h"
#include "SEImplementation/Plugin/FlexibleModelFitting/FlexibleModelFittingParameterManager.h"

#include "SEImplementation/Plugin/FlexibleModelFitting/FlexibleModelFittingPrior.h"

namespace SExtractor {

using namespace ModelFitting;

void FlexibleModelFittingPrior::setupPrior(
    FlexibleModelFittingParameterManager& parameter_manager, const SourceInterface& source,
    ResidualEstimator& residual_estimator) const {
  std::lock_guard<std::mutex> guard {python_callback_mutex};

  double expected_value = m_expected_value(source);
  double weight = 1. / m_standard_deviation(source);

  auto prior_residual = std::unique_ptr<WorldValueResidual>(
      new WorldValueResidual(*parameter_manager.getParameter(source, m_parameter), expected_value, weight));
  residual_estimator.registerBlockProvider(std::move(prior_residual));
}

}


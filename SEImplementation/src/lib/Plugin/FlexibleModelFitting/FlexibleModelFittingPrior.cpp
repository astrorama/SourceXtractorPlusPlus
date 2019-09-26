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
  double expected_value = m_expected_value(source);
  double weight = 1. / m_standard_deviation(source);

  auto prior_residual = std::unique_ptr<WorldValueResidual>(
      new WorldValueResidual(*parameter_manager.getParameter(source, m_parameter), expected_value, weight));
  residual_estimator.registerBlockProvider(std::move(prior_residual));
}

}


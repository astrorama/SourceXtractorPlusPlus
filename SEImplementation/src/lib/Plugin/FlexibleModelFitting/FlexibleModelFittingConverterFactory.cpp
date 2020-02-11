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
 * FlexibleModelFittingConverterFactory.cpp
 *
 *  Created on: May 10, 2019
 *      Author: mschefer
 */

#include "AlexandriaKernel/memory_tools.h"

#include "ModelFitting/Parameters/ExpSigmoidConverter.h"
#include "ModelFitting/Parameters/SigmoidConverter.h"
#include "ModelFitting/Parameters/NormalizedConverter.h"

#include "SEImplementation/Plugin/FlexibleModelFitting/FlexibleModelFittingConverterFactory.h"
#include "SEUtils/Python.h"

namespace SourceXtractor {

using namespace ModelFitting;
using Euclid::make_unique;

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


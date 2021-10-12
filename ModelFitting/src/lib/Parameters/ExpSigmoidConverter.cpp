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
/**
 * @file ExpSigmoidConverter.cpp
 *
 * Created on: August 12, 2015
 *     Author: Pierre Dubath
 */
#include <cmath>
#include <iostream>

#include <ElementsKernel/Logging.h>

#include "ModelFitting/Parameters/ExpSigmoidConverter.h"

namespace ModelFitting {

using namespace std;

static Elements::Logging logger = Elements::Logging::getLogger("ModelFitting");

ExpSigmoidConverter::~ExpSigmoidConverter() = default;

double ExpSigmoidConverter::worldToEngine(const double world_value) const {
  if (world_value < m_min_value || world_value > m_max_value) {
    logger.warn() << "WorldToEngine ExpSigmoidConverter: world values outside of possible range";
  }

  auto wv = std::max(m_min_value, std::min(m_max_value, world_value));
  return log(log(wv / m_min_value) / log(m_max_value / wv));
}


double ExpSigmoidConverter::engineToWorld(const double engine_value) const {
  auto clamped_value = std::max(-50.0, std::min(50.0, engine_value));
  return m_min_value * exp( log(m_max_value / m_min_value) / (1 + exp(-clamped_value)) );
}

double ExpSigmoidConverter::getEngineToWorldDerivative(const double value) const {
  return value * log(value / m_min_value) * log(m_max_value / value) / log(m_max_value / m_min_value);
}


}// namespace ModelFitting

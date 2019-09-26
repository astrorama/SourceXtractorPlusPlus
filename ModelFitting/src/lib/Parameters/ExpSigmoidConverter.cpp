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

#include "ModelFitting/Parameters/ExpSigmoidConverter.h"

namespace ModelFitting {

using namespace std;

ExpSigmoidConverter::~ExpSigmoidConverter() = default;

double ExpSigmoidConverter::worldToEngine(const double world_value) const {
  if (world_value < m_min_value || world_value > m_max_value) {
    throw Elements::Exception()
        << "WorldToEngine ExpSigmoidConverter: world values outside of possible range";
  }
  return log(log(world_value / m_min_value) / log(m_max_value / world_value));
}


double ExpSigmoidConverter::engineToWorld(const double engine_value) const {
  return m_min_value * exp( log(m_max_value / m_min_value) / (1 + exp(-engine_value)) );
}

double ExpSigmoidConverter::getEngineToWorldDerivative(const double value) const {
  return value * log(value - m_min_value) * log(m_max_value - value) / log(m_max_value - m_min_value);
}


}// namespace ModelFitting

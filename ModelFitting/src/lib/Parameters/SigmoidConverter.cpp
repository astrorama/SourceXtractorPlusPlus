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
 * @file SigmoidConverter.cpp
 *
 * Created on: August 12, 2015
 *     Author: Pierre Dubath
 */
#include <cmath>
#include <iostream>

#include "ModelFitting/Parameters/SigmoidConverter.h"

namespace ModelFitting {

using namespace std;

SigmoidConverter::~SigmoidConverter() = default;

double SigmoidConverter::worldToEngine(const double world_value) const {
  if (world_value < m_min_value || world_value > m_max_value) {
    throw Elements::Exception()
        << "WorldToEngine SigmoidConverter: world values outside of possible range";
  }

  double num = world_value - m_min_value;
  double den = m_max_value - world_value;
  return (num > 1e-50 ? (den > 1e-50 ? log(num/den) : 50.0) : -50.0);
}

double SigmoidConverter::engineToWorld(const double engine_value) const {
  auto clamped_value = std::max(-50.0, std::min(50.0, engine_value));
  return m_min_value + (m_max_value - m_min_value) / (1 + exp(-clamped_value));
}

double SigmoidConverter::getEngineToWorldDerivative(const double value) const {
  return (value - m_min_value) * (m_max_value - value) / (m_max_value - m_min_value);
}


}// namespace ModelFitting

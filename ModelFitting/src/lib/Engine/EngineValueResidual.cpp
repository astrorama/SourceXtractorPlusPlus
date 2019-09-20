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
 * @file EngineValueResidual.cpp
 * @date August 18, 2015
 * @author Nikolaos Apostolakos
 */

#include "ModelFitting/Engine/EngineValueResidual.h"

namespace ModelFitting {

EngineValueResidual::~EngineValueResidual() = default;

static double computeResidual(double current, double expected, double weight) {
  return weight * (expected - current);
}

EngineValueResidual::EngineValueResidual(EngineParameter& parameter,
                                       double expected_value, double weight)
        : m_residual{computeResidual(parameter.getEngineValue(), expected_value, weight)} {
  parameter.addObserver(
      [this, &parameter, expected_value, weight](double){
        m_residual = computeResidual(parameter.getEngineValue(), expected_value, weight);
      }
  );
}

std::size_t EngineValueResidual::numberOfResiduals() const {
  return 1;
}
  
void EngineValueResidual::populateResidualBlock(IterType output_iter) {
  *output_iter = m_residual;
}

} // end of namespace ModelFitting
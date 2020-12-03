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
 * @file WorldValueResidual.cpp
 * @date August 14, 2015
 * @author Nikolaos Apostolakos
 */

#include "ModelFitting/Engine/WorldValueResidual.h"

namespace ModelFitting {

WorldValueResidual::~WorldValueResidual() {
  m_parameter->removeObserver(m_observer_id);
}

static double computeResidual(double current, double expected, double weight) {
  return weight * (current - expected);
}

WorldValueResidual::WorldValueResidual(std::shared_ptr<BasicParameter> parameter,
                                       double expected_value, double weight)
        : m_parameter(parameter), m_residual{computeResidual(parameter->getValue(), expected_value, weight)} {
    m_observer_id = parameter->addObserver(
      [this, expected_value, weight](double new_value){
        m_residual = computeResidual(new_value, expected_value, weight);
      }
  );
}

std::size_t WorldValueResidual::numberOfResiduals() const {
  return 1;
}
  
void WorldValueResidual::populateResidualBlock(IterType output_iter) {
  *output_iter = m_residual;
}

} // end of namespace ModelFitting

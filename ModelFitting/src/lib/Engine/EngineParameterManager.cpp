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
 * @file EngineParameterManager.cpp
 * @date August 14, 2015
 * @author Nikolaos Apostolakos
 */

#include "ModelFitting/Engine/EngineParameterManager.h"

namespace ModelFitting {

EngineParameterManager::~EngineParameterManager() = default;

void EngineParameterManager::registerParameter(EngineParameter parameter) {
  m_parameters.emplace_back(std::move(parameter));
}

std::size_t EngineParameterManager::numberOfParameters() {
  return m_parameters.size();
}

std::vector<double> EngineParameterManager::convertCovarianceMatrixToWorldSpace(std::vector<double> covariance_matrix) const {
  std::vector<double> converted_matrix;
  converted_matrix.reserve(covariance_matrix.size());

  auto iter = covariance_matrix.begin();
  for (int j=0; j*j < covariance_matrix.size(); j++) {
    for (int i=0; i*i < covariance_matrix.size(); i++) {
      converted_matrix.push_back(*iter *
          m_parameters[i].getEngineToWorldDerivative() * m_parameters[j].getEngineToWorldDerivative());

      iter++;
    }
  }
  return converted_matrix;
}


} // end of namespace ModelFitting

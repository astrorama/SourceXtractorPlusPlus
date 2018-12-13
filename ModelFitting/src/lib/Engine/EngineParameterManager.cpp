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

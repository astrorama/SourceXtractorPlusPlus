/** 
 * @file EngineParameterManager.cpp
 * @date August 14, 2015
 * @author Nikolaos Apostolakos
 */

#include "ModelFitting/Engine/EngineParameterManager.h"

namespace ModelFitting {

EngineParameterManager::~EngineParameterManager() = default;

void EngineParameterManager::registerParameter(std::shared_ptr<EngineParameter> parameter) {
  m_parameters.emplace_back(std::move(parameter));
}

std::size_t EngineParameterManager::numberOfParameters() {
  return m_parameters.size();
}

} // end of namespace ModelFitting
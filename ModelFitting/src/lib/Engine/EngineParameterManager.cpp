/** 
 * @file EngineParameterManager.cpp
 * @date August 14, 2015
 * @author Nikolaos Apostolakos
 */

#include "ModelFitting/Engine/EngineParameterManager.h"

namespace ModelFitting {

EngineParameterManager::~EngineParameterManager() = default;

void EngineParameterManager::registerParameter(EngineParameter& parameter) {
  // Create a lambda function which can be used for retrieving the value
  // of the parameter
  m_getter_functions.emplace_back(
      [&parameter](){
        return parameter.getEngineValue();
      }
  );
  // Create a lambda function which can be used for setting the value
  // of the parameter
  m_setter_functions.emplace_back(
      [&parameter](double new_value){
        parameter.setEngineValue(new_value);
      }
  );
}

std::size_t EngineParameterManager::numberOfParameters() {
  return m_getter_functions.size();
}

} // end of namespace ModelFitting
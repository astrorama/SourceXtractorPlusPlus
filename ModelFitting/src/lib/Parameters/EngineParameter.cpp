/**
 * @file EngineParameter.cpp
 *
 * Created on: August 12, 2015
 *     Author: Pierre Dubath
 */

#include "ModelFitting/Parameters/EngineParameter.h"

namespace ModelFitting {

using namespace std;

void EngineParameter::setEngineValue(const double engine_value) {
  m_engine_value = engine_value;
  BasicParameter::setValue(m_converter->engineToWorld(engine_value));
}

} // namespace ModelFitting

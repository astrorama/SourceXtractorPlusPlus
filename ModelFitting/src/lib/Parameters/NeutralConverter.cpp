/**
 * @file NeutralConverter.cpp
 *
 * Created on: August 12, 2015
 *     Author: Pierre Dubath
 */
#include "ModelFitting/Parameters/NeutralConverter.h"

namespace ModelFitting {

using namespace std;

NeutralConverter::~NeutralConverter() = default;

double NeutralConverter::worldToEngine(const double world_value) const {
  return world_value;
}

double NeutralConverter::engineToWorld(const double engine_value) const {
  return engine_value;
}

double NeutralConverter::getEngineToWorldDerivative(const double /*value*/) const {
  return 1;
}


}// namespace ModelFitting

/** 
 * @file NormalizedConverter.cpp
 * @date August 18, 2015
 * @author Nikolaos Apostolakos
 */

#include "ModelFitting/Parameters/NormalizedConverter.h"

namespace ModelFitting {

NormalizedConverter::NormalizedConverter(double norm_value) : m_norm_value{norm_value} { }

NormalizedConverter::~NormalizedConverter() = default;

double NormalizedConverter::worldToEngine(const double world_value) const {
  return world_value / m_norm_value;
}

double NormalizedConverter::engineToWorld(const double engine_value) const {
  return engine_value * m_norm_value;
}

double NormalizedConverter::getEngineToWorldDerivative(const double /*value*/) const {
  return m_norm_value;
}

} // end of namespace ModelFitting

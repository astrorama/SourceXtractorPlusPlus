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
  //xmmin   = x[p] - xmin[p];
  //maxmx   = xmax[p] - x[p];
  //maxmmin = xmax[p] - xmin[p];
  //dxdy[f++] = xmmin * maxmx / maxmmin;
  return (value - m_min_value) * (m_max_value - value) / (m_max_value - m_min_value);
}


}// namespace ModelFitting

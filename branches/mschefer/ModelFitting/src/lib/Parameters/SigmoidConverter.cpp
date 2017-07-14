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

  //return log((world_value - m_min_value) / (m_max_value - world_value)) / m_scale_factor;

  double num = world_value - m_min_value;
  double den = m_max_value - world_value;
  //std::cout << "w2e " << world_value << " " << (num > 1e-50 ? (den > 1e-50 ? log10(num/den) : 50.0) : -50.0) << "\n";
  return (num > 1e-50 ? (den > 1e-50 ? log10(num/den) : 50.0) : -50.0) / m_scale_factor;
}

double SigmoidConverter::engineToWorld(const double engine_value) const {
//  std::cout << "e2w " << engine_value << " " << (m_min_value + (m_max_value - m_min_value) / (1 + exp10(-(engine_value > 50.0 ? 50.0
//      : (engine_value < -50.0 ? -50.0 : engine_value))))) << "\n";
  return m_min_value + (m_max_value - m_min_value) / (1 + exp10(-((engine_value * m_scale_factor) > 50.0 ? 50.0
      : ((engine_value * m_scale_factor) < -50.0 ? -50.0 : (engine_value * m_scale_factor)))));
//  return m_min_value + (m_max_value - m_min_value) / (1 + exp(-engine_value * m_scale_factor));
}

}// namespace ModelFitting

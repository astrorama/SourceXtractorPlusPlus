/**
 * @file SigmoidConverter.cpp
 *
 * Created on: August 12, 2015
 *     Author: Pierre Dubath
 */
#include <cmath>

#include "ModelFitting/Parameters/SigmoidConverter.h"

namespace ModelFitting {

using namespace std;

SigmoidConverter::~SigmoidConverter() = default;

double SigmoidConverter::worldToEngine(const double world_value) const {
  if (world_value < m_min_value || world_value > m_max_value) {
    throw Elements::Exception()
        << "WorldToEngine SigmoidConverter: world values outside of possible range";
  }
  return log((world_value - m_min_value) / (m_max_value - world_value));
}

double SigmoidConverter::engineToWorld(const double engine_value) const {
  return m_min_value + (m_max_value - m_min_value) / (1 + exp(-engine_value));
}

}// namespace ModelFitting

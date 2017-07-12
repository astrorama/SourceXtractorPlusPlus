/**
 * @file ExpSigmoidConverter.cpp
 *
 * Created on: August 12, 2015
 *     Author: Pierre Dubath
 */
#include <cmath>

#include "ModelFitting/Parameters/ExpSigmoidConverter.h"

namespace ModelFitting {

using namespace std;

ExpSigmoidConverter::~ExpSigmoidConverter() = default;

double ExpSigmoidConverter::worldToEngine(const double world_value) const {
  if (world_value < m_min_value || world_value > m_max_value) {
    throw Elements::Exception()
        << "WorldToEngine ExpSigmoidConverter: world values outside of possible range";
  }
  return (log(world_value / m_min_value) / log((m_max_value / world_value))) / m_scale_factor;
}


double ExpSigmoidConverter::engineToWorld(const double engine_value) const {
  return m_min_value * exp( log(m_max_value / m_min_value) / (1 + exp(-engine_value * m_scale_factor)) );
}


}// namespace ModelFitting

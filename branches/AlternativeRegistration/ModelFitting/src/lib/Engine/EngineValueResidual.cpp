/** 
 * @file EngineValueResidual.cpp
 * @date August 18, 2015
 * @author Nikolaos Apostolakos
 */

#include "ModelFitting/Engine/EngineValueResidual.h"

namespace ModelFitting {

EngineValueResidual::~EngineValueResidual() = default;

static double computeResidual(double current, double expected, double weight) {
  return weight * (expected - current);
}

EngineValueResidual::EngineValueResidual(EngineParameter& parameter,
                                       double expected_value, double weight)
        : m_residual{computeResidual(parameter.getEngineValue(), expected_value, weight)} {
  parameter.addObserver(
      [this, &parameter, expected_value, weight](double){
        m_residual = computeResidual(parameter.getEngineValue(), expected_value, weight);
      }
  );
}

std::size_t EngineValueResidual::numberOfResiduals() const {
  return 1;
}
  
void EngineValueResidual::populateResidualBlock(IterType output_iter) {
  *output_iter = m_residual;
}

} // end of namespace ModelFitting
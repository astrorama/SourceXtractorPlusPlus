/** 
 * @file WorldValueResidual.cpp
 * @date August 14, 2015
 * @author Nikolaos Apostolakos
 */

#include "ModelFitting/Engine/WorldValueResidual.h"

namespace ModelFitting {

WorldValueResidual::~WorldValueResidual() = default;

static double computeResidual(double current, double expected, double weight) {
  return weight * (current - expected);
}

WorldValueResidual::WorldValueResidual(BasicParameter& parameter,
                                       double expected_value, double weight)
        : m_residual{computeResidual(parameter.getValue(), expected_value, weight)} {
  parameter.addObserver(
      [this, expected_value, weight](double new_value){
        m_residual = computeResidual(new_value, expected_value, weight);
      }
  );
}

std::size_t WorldValueResidual::numberOfResiduals() const {
  return 1;
}
  
void WorldValueResidual::populateResidualBlock(IterType output_iter) {
  *output_iter = m_residual;
}

} // end of namespace ModelFitting
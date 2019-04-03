/**
 * @file GSLEngine.cpp
 * @date April, 2019
 * @author Alejandro Alvarez Ayllon
 */

#include <ElementsKernel/Exception.h>
#include "ModelFitting/Engine/GSLEngine.h"

namespace ModelFitting {

#ifndef WITHOUT_GSL
static std::shared_ptr<LeastSquareEngine> createLevmarEngine() {
  return std::make_shared<GSLEngine>();
}

static LeastSquareEngine::StaticEngine levmar_engine{"GSL", createLevmarEngine};
#endif

GSLEngine::GSLEngine() {
}

GSLEngine::~GSLEngine() {
}

LeastSquareSummary GSLEngine::solveProblem(ModelFitting::EngineParameterManager&,
                                           ModelFitting::ResidualEstimator&) {
  throw Elements::Exception() << "GSL Engine not implemented";
}

} // end namespace ModelFitting

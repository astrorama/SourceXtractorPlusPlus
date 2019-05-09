/**
 * @file CeresEngine.h
 * @date May, 2019
 * @author Alejandro Alvarez Ayllon
 */

#ifndef MODELFITTING_CERESENGINE_H
#define        MODELFITTING_CERESENGINE_H

#include "ModelFitting/Engine/LeastSquareEngine.h"

namespace ModelFitting {

class CeresEngine : public LeastSquareEngine {
public:

  CeresEngine(int itmax = 1000, double gtol = 1e-10, double ftol = 1e-8, double ptol = 1e-8, double delta = 1e-4);

  virtual ~CeresEngine() = default;

  LeastSquareSummary solveProblem(EngineParameterManager& parameter_manager,
                                  ResidualEstimator& residual_estimator) override;

private:
  int m_itmax;
  double m_gtol, m_ftol, m_ptol, m_delta;
};

} // end of namespace ModelFitting

#endif // MODELFITTING_CERESENGINE_H

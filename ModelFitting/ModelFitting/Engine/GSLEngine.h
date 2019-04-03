/**
 * @file GSLEngine.h
 * @date April, 2019
 * @author Alejandro Alvarez Ayllon
 */

#ifndef MODELFITTING_GSLENGINE_H
#define	MODELFITTING_GSLENGINE_H

#include "ModelFitting/Engine/LeastSquareEngine.h"

namespace ModelFitting {

/**
 * @class GSLEngine
 *
 * @brief
 * LeastSquareEngine implementation which uses the GNU Scientific Library
 */
class GSLEngine : public LeastSquareEngine {

public:

  /**
   * @brief Constructs a new instance of the engine
   */
  GSLEngine();

  /// Destructor
  virtual ~GSLEngine();

  /// Solves the minimization problem using the levmar library. The returned
  /// summary contains as underlying_framework_info an std::array<double,10>,
  /// which keeps the information regarding the minimization as it is provided
  /// by levmar (for more info see http://users.ics.forth.gr/~lourakis/levmar).
  LeastSquareSummary solveProblem(EngineParameterManager& parameter_manager,
                                  ResidualEstimator& residual_estimator) override;

private:
};

} // end of namespace ModelFitting

#endif	/* MODELFITTING_LEVMARENGINE_H */

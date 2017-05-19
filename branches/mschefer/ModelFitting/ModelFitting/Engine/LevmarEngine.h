/** 
 * @file LevmarEngine.h
 * @date August 17, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef MODELFITTING_LEVMARENGINE_H
#define	MODELFITTING_LEVMARENGINE_H

#include "ModelFitting/Engine/LeastSquareEngine.h"

namespace ModelFitting {

/**
 * @class LevmarEngine
 * 
 * @brief
 * LeastSquareEngine implementation which uses the levmar library
 */
class LevmarEngine : public LeastSquareEngine {
  
public:
  
  /**
   * @brief Constructs a new instance of the engine
   * 
   * @param itmax
   *    maximum number of iterations
   * @param tau
   *    \f$\tau\f$: scale factor for initial \f$\mu\f$
   * @param epsilon1
   *    \f$\epsilon1\f$: stopping threshold for \f$||J^T\epsilon||_{inf}\f$
   * @param epsilon2
   *    \f$\epsilon1\f$: stopping threshold for \f$||\Delta p||_2\f$
   * @param epsilon3
   *    \f$\epsilon1\f$: stopping threshold for \f$||\epsilon||_2\f$
   * @param delta
   *    step used in difference approximation to the Jacobian
   */
  LevmarEngine(size_t itmax=1000, double tau=1E-3, double epsilon1=1E-8,
               double epsilon2=1E-8, double epsilon3=1E-8, double delta=1E-4);
  
  /// Destructor
  virtual ~LevmarEngine();
  
  /// Solves the minimization problem using the levmar library. The returned
  /// summary contains as underlying_framework_info an std::array<double,10>,
  /// which keeps the information regarding the minimization as it is provided
  /// by levmar (for more info see http://users.ics.forth.gr/~lourakis/levmar).
  LeastSquareSummary solveProblem(EngineParameterManager& parameter_manager,
                                  ResidualEstimator& residual_estimator) override;
  
private:
  
  size_t m_itmax;
  std::vector<double> m_opts;
  
};

} // end of namespace ModelFitting

#endif	/* MODELFITTING_LEVMARENGINE_H */


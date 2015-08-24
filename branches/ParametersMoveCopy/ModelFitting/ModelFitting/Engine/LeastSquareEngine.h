/** 
 * @file LeastSquareEngine.h
 * @date August 17, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef MODELFITTING_LEASTSQUAREENGINE_H
#define	MODELFITTING_LEASTSQUAREENGINE_H

#include "ModelFitting/Engine/EngineParameterManager.h"
#include "ModelFitting/Engine/ResidualEstimator.h"
#include "ModelFitting/Engine/LeastSquareSummary.h"

namespace ModelFitting {

/**
 * @class LeastSquareEngine
 * 
 * @brief
 * Interface of classes implementing least square minimization
 * 
 * @details
 * The implementations of this interface should implement the solveProblem()
 * method to solve the minimization problem, using an underlying minimization
 * library. The given arguments (an EngineParameterManager and a ResidualEstimator)
 * should be used the following way:
 * 
 * - The number of the free parameters can be retrieved by using the method
 *   EngineParameterManager::numberOfParameters()
 * - The number of residuals can be retrieved by using the method
 *   ResidualEstimator::numberOfResiduals()
 * - The initial parameters can be retrieved by using the method
 *   EngineParameterManager::getEngineValues()
 * - In each call of the cost function of the underlying library, two things
 *   need to be done:
 *   - Update the managed parameter values by using the method
 *     EngineParameterManager::updateEngineValues()
 *   - Get the residual values by using the method ResidualEstimator::populateResiduals()
 * - After the minimization is done, the engine must guarantee that the managed
 *   parameters have been updated with the results of the minimization by using
 *   the EngineParameterManager::updateEngineValues() method
 */
class LeastSquareEngine {
  
public:
  
  /// Destructor
  virtual ~LeastSquareEngine() = default;
  
  /// Solves the problem defined by the given parameter manager and residual
  /// estimator.
  virtual LeastSquareSummary solveProblem(EngineParameterManager& parameter_manager,
                                          ResidualEstimator& residual_estimator) = 0;
  
};

} // end of namespace ModelFitting

#endif	/* MODELFITTING_LEASTSQUAREENGINE_H */


/** Copyright © 2019 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3.0 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */
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

  using FactoryMethod = std::function<std::shared_ptr<LeastSquareEngine>()>;

  static void registerEngine(const std::string& name, FactoryMethod factory_method);

  static std::vector<std::string> getImplementations();

  static std::shared_ptr<LeastSquareEngine> create(const std::string &name);

  struct StaticEngine {
    StaticEngine(const std::string& name, LeastSquareEngine::FactoryMethod factory_method) {
      LeastSquareEngine::registerEngine(name, factory_method);
    }
  };
};

} // end of namespace ModelFitting

#endif	/* MODELFITTING_LEASTSQUAREENGINE_H */


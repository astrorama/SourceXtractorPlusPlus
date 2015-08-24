/** 
 * @file EngineParameterManager.h
 * @date August 13, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef MODELFITTING_ENGINEPARAMETERMANAGER_H
#define	MODELFITTING_ENGINEPARAMETERMANAGER_H

#include <vector>
#include <functional>
#include "ModelFitting/Parameters/EngineParameter.h"

namespace ModelFitting {

/**
 * @class EngineParameterManager
 * 
 * @brief
 * Class responsible for managing the parameters the least square engine minimizes
 * 
 * @details
 * The typical usage of this manager is to create an instance with the default
 * constructor, register the parameters to be minimized by using the
 * registerParameter() function, and pass the manager to the LeastSquareEngine.
 * For example:
 * \code{.cpp}
 * // The parameters which will be optimized
 * EngineParameter param1 = ...;
 * EngineParameter param2 = ...;
 * EngineParameter param3 = ...;
 * 
 * // Create the manager and register the parameters
 * EngineParameterManager manager {};
 * manager.registerParameter(param1);
 * manager.registerParameter(param2);
 * manager.registerParameter(param3);
 * 
 * // Pass the parameter to an engine for solving a problem
 * engine.solveProblem(manager, residual_estimator);
 * \endcode
 * 
 * The purpose of the rest of the methods of this class is to be used by the
 * minimization engine and can be safely ignored by the normal user.
 */
class EngineParameterManager {
  
public:
  
  /// Destructor
  virtual ~EngineParameterManager();
  
  /**
   * @brief Registers an EngineParameter to the EngineParameterManager
   * 
   * @details
   * Registered parameters are the ones which will be optimized by the
   * minimization engine, while solving the least square problem. The manager
   * will update their values during the minimization, as requested by the
   * engine.
   * 
   * Each parameter to be optimized by the minimization engine should be
   * registered by a call to this method a single time, before passing the
   * manager to the engine.
   * 
   * Note that the EngineParameterManager relies on the fact that the passed
   * reference lifetime is longer than the one of the manager, in other words,
   * that the reference will be valid for the full time of the minimization
   * solving. Failure to meet this requirement leads to undefined behavior.
   * 
   * @param parameter
   *    The EngineParameter to register to the manager
   */
  void registerParameter(EngineParameter& parameter);
  
  /// Returns the number of parameters managed by the manager
  std::size_t numberOfParameters();
  
  /**
   * @brief Returns the engine values of the managed parameters
   * 
   * @details
   * The engine values are returned via the passed iterator. The order of the
   * values is the same with the order the parameters were registered to the
   * manager (by calls to the registerParameter() method).
   * 
   * It is the responsibility of the caller to make sure that the structure
   * where the iterator points has enough elements to accommodate the results.
   * Failure of this precondition can lead to undefined behavior.
   * 
   * @tparam DoubleIter
   *    A forward output iterator to double
   * @param output_iter
   *    An iterator pointing to the first element where the parameter engine
   *    values will be outputed
   */
  template <typename DoubleIter>
  void getEngineValues(DoubleIter output_iter) const;
  
  /**
   * @brief Updates the managed parameters with the given engine values
   * 
   * @details
   * The engine values of the parameters are retrieved using the passed iterator.
   * The order of the values is assumed to be the same as the order the
   * parameters were registered to the manager.
   * 
   * It is the responsibility of the caller to make sure that the structure
   * where the iterator points contains at least as many elements as the number
   * of the managed parameters. Failure of this precondition can lead to
   * undefined behavior.
   * 
   * @tparam DoubleIter
   *    A forward input iterator to double
   * @param new_values_iter
   */
  template <typename DoubleIter>
  void updateEngineValues(DoubleIter new_values_iter);
  
private:
  
  std::vector<std::function<double()>> m_getter_functions {};
  std::vector<std::function<void(double)>> m_setter_functions {};
  
}; // end of class EngineParameterManager

} // end of namespace ModelFitting

// Include the templates implementation
#include "_impl/EngineParameterManager.icpp"

#endif	/* MODELFITTING_ENGINEPARAMETERMANAGER_H */


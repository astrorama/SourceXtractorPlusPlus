/** 
 * @file LevmarEngine.cpp
 * @date August 17, 2015
 * @author Nikolaos Apostolakos
 */

#include <mutex>

#include <levmar.h>
#include "ModelFitting/Engine/LevmarEngine.h"

#include <iostream>

namespace ModelFitting {

LevmarEngine::LevmarEngine(size_t itmax, double tau, double epsilon1,
               double epsilon2, double epsilon3, double delta)
      : m_itmax{itmax}, m_opts{tau, epsilon1, epsilon2, epsilon3, delta} { }
      
LevmarEngine::~LevmarEngine() = default;

// The Levmar library seems to have some problems with multithreading, this mutex is used to ensure only one thread
// in levmar
namespace {
  std::mutex levmar_mutex;
}

LeastSquareSummary LevmarEngine::solveProblem(EngineParameterManager& parameter_manager,
                                              ResidualEstimator& residual_estimator) {
  // Create a tuple which keeps the references to the given manager and estimator
  auto adata = std::tie(parameter_manager, residual_estimator);

  // The function which is called by the levmar loop
  auto levmar_res_func = [](double *p, double *hx, int, int, void *extra) {
    levmar_mutex.unlock();

    auto* extra_ptr = (decltype(adata)*)extra;
    EngineParameterManager& pm = std::get<0>(*extra_ptr);
    pm.updateEngineValues(p);
    ResidualEstimator& re = std::get<1>(*extra_ptr);
    re.populateResiduals(hx);

    levmar_mutex.lock();
  };

  // Create the vector which will be used for keeping the parameter values
  // and initialize it to the current values of the parameters
  std::vector<double> param_values (parameter_manager.numberOfParameters());
  parameter_manager.getEngineValues(param_values.begin());

  // Create a vector for getting the information of the minimization
  std::array<double, 10> info;

  levmar_mutex.lock();
  // Call the levmar library
  auto res = dlevmar_dif(levmar_res_func, // The function called from the levmar algorithm
                         param_values.data(), // The pointer where the parameter values are
                         NULL, // We don't use any measurement vector
                         parameter_manager.numberOfParameters(), // The number of free parameters
                         residual_estimator.numberOfResiduals(), // The number of residuals
                         m_itmax, // The maximum number of iterations
                         m_opts.data(), // The minimization options
                         info.data(), // Where the information of the minimization is stored
                         NULL, // Working memory is allocated internally
                         NULL, // Ignoring covariance for the moment
                         &adata // No additional data needed
                        );
  levmar_mutex.unlock();
  
  // Create and return the summary object
  LeastSquareSummary summary {};
  summary.success_flag = (res != -1);
  summary.iteration_no = info[5];
  summary.underlying_framework_info = info;
  return summary;
}

} // end of namespace ModelFitting

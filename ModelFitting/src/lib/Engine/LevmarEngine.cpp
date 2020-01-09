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
 * @file LevmarEngine.cpp
 * @date August 17, 2015
 * @author Nikolaos Apostolakos
 */

#include <cmath>
#include <mutex>

#include <levmar.h>
#include <ElementsKernel/Exception.h>
#include "ModelFitting/Engine/LeastSquareEngineManager.h"
#include "ModelFitting/Engine/LevmarEngine.h"


namespace ModelFitting {

static std::shared_ptr<LeastSquareEngine> createLevmarEngine(unsigned max_iterations) {
  return std::make_shared<LevmarEngine>(max_iterations);
}

static LeastSquareEngineManager::StaticEngine levmar_engine{"levmar", createLevmarEngine};

LevmarEngine::LevmarEngine(size_t itmax, double tau, double epsilon1,
               double epsilon2, double epsilon3, double delta)
      : m_itmax{itmax}, m_opts{tau, epsilon1, epsilon2, epsilon3, delta} { }

LevmarEngine::~LevmarEngine() = default;


#ifdef LINSOLVERS_RETAIN_MEMORY
// If the Levmar library is not configured for multithreading, this mutex is used to ensure only one thread
// at a time can enter levmar
namespace {
  std::mutex levmar_mutex;
}
#endif

LeastSquareSummary LevmarEngine::solveProblem(EngineParameterManager& parameter_manager,
                                              ResidualEstimator& residual_estimator) {
  // Create a tuple which keeps the references to the given manager and estimator
  auto adata = std::tie(parameter_manager, residual_estimator);

  // The function which is called by the levmar loop
  auto levmar_res_func = [](double *p, double *hx, int, int, void *extra) {
#ifdef LINSOLVERS_RETAIN_MEMORY
    levmar_mutex.unlock();
#endif
    auto* extra_ptr = (decltype(adata)*)extra;
    EngineParameterManager& pm = std::get<0>(*extra_ptr);
    pm.updateEngineValues(p);
    ResidualEstimator& re = std::get<1>(*extra_ptr);
    re.populateResiduals(hx);

#ifdef LINSOLVERS_RETAIN_MEMORY
    levmar_mutex.lock();
#endif
    };

  // Create the vector which will be used for keeping the parameter values
  // and initialize it to the current values of the parameters
  std::vector<double> param_values (parameter_manager.numberOfParameters());
  parameter_manager.getEngineValues(param_values.begin());

  // Create a vector for getting the information of the minimization
  std::array<double, 10> info;

  std::vector<double> covariance_matrix (parameter_manager.numberOfParameters() * parameter_manager.numberOfParameters());

#ifdef LINSOLVERS_RETAIN_MEMORY
  levmar_mutex.lock();
#endif
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
                         covariance_matrix.data(),
                         &adata // No additional data needed
                        );
#ifdef LINSOLVERS_RETAIN_MEMORY
  levmar_mutex.unlock();
#endif

  // Create and return the summary object
  LeastSquareSummary summary {};

  auto converted_covariance_matrix = parameter_manager.convertCovarianceMatrixToWorldSpace(covariance_matrix);
  for (unsigned int i=0; i<parameter_manager.numberOfParameters(); i++) {
    summary.parameter_sigmas.push_back(sqrt(converted_covariance_matrix[i*(parameter_manager.numberOfParameters()+1)]));
  }

  summary.success_flag = (res != -1);
  summary.iteration_no = info[5];
  summary.underlying_framework_info = info;
  return summary;
}

} // end of namespace ModelFitting

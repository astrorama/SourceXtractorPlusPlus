/**
 * @file CeresEngine.cpp
 * @date May, 2019
 * @author Alejandro Alvarez Ayllon
 */

#include "ModelFitting/Engine/CeresEngine.h"
#include "ModelFitting/Engine/LeastSquareEngineManager.h"
#include "ModelFitting/utils.h"
#include <ceres/ceres.h>
#include <iomanip>
#include <ElementsKernel/Exception.h>
#include <ElementsKernel/Logging.h>

namespace ModelFitting {

static Elements::Logging logger = Elements::Logging::getLogger("CeresEngine");

using namespace ceres;

static std::shared_ptr<LeastSquareEngine> createCeresEngine(unsigned max_iterations) {
  return std::make_shared<CeresEngine>(max_iterations);
}

static LeastSquareEngineManager::StaticEngine ceres_engine{"ceres", createCeresEngine};

CeresEngine::CeresEngine(int itmax, double gtol, double ftol, double ptol, double delta)
: m_itmax{itmax}, m_gtol{gtol}, m_ftol{ftol}, m_ptol{ptol}, m_delta{delta} {
}

class CeresCallback: public IterationCallback {
public:
  CeresCallback(EngineParameterManager& parameter_manager): m_parameter_manager(parameter_manager) {
  }

  CallbackReturnType operator()(const IterationSummary& summary) override {
    std::cout << summary.iteration << '\t' << std::fixed << std::setprecision(6);
    std::vector<double> params(m_parameter_manager.numberOfParameters());
    m_parameter_manager.getEngineValues(params.begin());
    for (auto &p : params) {
      std::cout << p << '\t';
    }
    std::cout << summary.cost * 2 << std::endl;
    return SOLVER_CONTINUE;
  }

private:
  EngineParameterManager& m_parameter_manager;
};

// Function to be minimized
class CostFunctor {
public:
  CostFunctor(EngineParameterManager& parameter_manager, ResidualEstimator& residual_estimator)
    : m_parameter_manager(parameter_manager), m_residual_estimator(residual_estimator), m_n_eval(0) {
  }

  bool operator()(double const* const *parameters, double *residuals) const {
    m_parameter_manager.updateEngineValues(parameters[0]);
    m_residual_estimator.populateResiduals(residuals);
    ++m_n_eval;
    return true;
  }

  int getEvaluationCount() const {
    return m_n_eval;
  }

private:
  EngineParameterManager &m_parameter_manager;
  ResidualEstimator& m_residual_estimator;
  mutable int m_n_eval;
};

LeastSquareSummary CeresEngine::solveProblem(EngineParameterManager& parameter_manager,
                                             ResidualEstimator& residual_estimator) {
  // Initial guess
  std::vector<double> param_values(parameter_manager.numberOfParameters());
  parameter_manager.getEngineValues(param_values.begin());

  // Set up Ceres problem solver
  Problem problem;
  NumericDiffOptions diff_options;
  Solver::Options options;
  Solver::Summary ceres_summary;

  diff_options.relative_step_size = m_delta;

  options.minimizer_type = TRUST_REGION;
  options.linear_solver_type = DENSE_NORMAL_CHOLESKY;
  options.trust_region_strategy_type = LEVENBERG_MARQUARDT;
  options.jacobi_scaling = false;
  options.preconditioner_type = JACOBI;

  options.gradient_tolerance = m_gtol;
  options.function_tolerance = m_ftol;
  options.parameter_tolerance = m_ptol;
  options.max_num_iterations = m_itmax;

  //CeresCallback callback(parameter_manager);
  //options.update_state_every_iteration = true;
  //options.callbacks.emplace_back(&callback);

  auto cost_eval = new CostFunctor(parameter_manager, residual_estimator);
  auto cost_function = new DynamicNumericDiffCostFunction<CostFunctor, FORWARD>(cost_eval, TAKE_OWNERSHIP, diff_options);

  cost_function->AddParameterBlock(parameter_manager.numberOfParameters());
  cost_function->SetNumResiduals(residual_estimator.numberOfResiduals());

  // Problem gets the ownership!
  problem.AddResidualBlock(cost_function, nullptr, param_values.data());

  // Solve
  Solve(options, &problem, &ceres_summary);
  logger.debug() << ceres_summary.FullReport();

  // Prepare summary
  LeastSquareSummary summary;
  summary.iteration_no = ceres_summary.iterations.size();
  summary.success_flag = ceres_summary.IsSolutionUsable();

  int reason = 0;
  switch (ceres_summary.termination_type) {
    case USER_SUCCESS:
    case CONVERGENCE:
      reason = 1;
      break;
    case NO_CONVERGENCE:
      reason = 3;
      break;
    case USER_FAILURE:
    case FAILURE:
      reason = 7;
  }

  // Sigmas
  Covariance::Options covariance_options;
  covariance_options.algorithm_type = DENSE_SVD;
  covariance_options.null_space_rank = -1;
  Covariance covariance{covariance_options};
  std::vector<double> covariance_matrix(parameter_manager.numberOfParameters() * parameter_manager.numberOfParameters());

  std::vector<std::pair<const double*, const double*> > covariance_blocks;
  covariance_blocks.push_back(std::make_pair(param_values.data(), param_values.data()));

  if (!covariance.Compute(covariance_blocks, &problem)) {
    throw Elements::Exception() << "Failed to compute the covariance matrix";
  }

  covariance.GetCovarianceBlock(param_values.data(), param_values.data(), covariance_matrix.data());

  auto converted_covariance_matrix = parameter_manager.convertCovarianceMatrixToWorldSpace(covariance_matrix);
  for (unsigned int i=0; i<parameter_manager.numberOfParameters(); i++) {
    summary.parameter_sigmas.push_back(::sqrt(converted_covariance_matrix[i*(parameter_manager.numberOfParameters()+1)]));
  }

  // Levmar-compatible engine info
  summary.underlying_framework_info = std::array<double, 10>{
    ceres_summary.initial_cost * 2, // ||e||_2 at initial p
    ceres_summary.final_cost * 2,  // ||e||_2
    0, // ||J^T e||_inf
    0, // ||Dp||_2
    0., // mu/max[J^T J]_ii
    static_cast<double>(summary.iteration_no), // Number of iterations
    static_cast<double>(reason), // Reason for finishing
    static_cast<double>(cost_eval->getEvaluationCount()), // Function evaluations
    0., // Jacobian evaluations
    0. // Linear systems solved
  };

  return summary;
}

}  // end of namespace ModelFitting

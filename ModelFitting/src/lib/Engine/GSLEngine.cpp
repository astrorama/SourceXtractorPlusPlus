/**
 * @file GSLEngine.cpp
 * @date April, 2019
 * @author Alejandro Alvarez Ayllon
 */

#include <gsl/gsl_multifit_nlinear.h>
#include <gsl/gsl_blas.h>
#include <ElementsKernel/Exception.h>
#include "ModelFitting/Engine/GSLEngine.h"


namespace ModelFitting {


static std::shared_ptr<LeastSquareEngine> createLevmarEngine(unsigned max_iterations) {
  return std::make_shared<GSLEngine>(max_iterations);
}

static LeastSquareEngine::StaticEngine levmar_engine{"gsl", createLevmarEngine};

GSLEngine::GSLEngine(int itmax, double xtol, double gtol, double ftol):
  m_itmax{itmax}, m_xtol{xtol}, m_gtol{gtol}, m_ftol{ftol} {
}

// Provide an iterator for gsl_vector
class GslVectorIterator: public std::iterator<std::output_iterator_tag, double> {
private:
  gsl_vector *m_v;
  size_t m_i;

public:

  GslVectorIterator(gsl_vector *v): m_v{v}, m_i{0} {}

  GslVectorIterator(const GslVectorIterator&) = default;

  GslVectorIterator& operator++() {
    ++m_i;
    return *this;
  }

  GslVectorIterator operator++(int) {
    auto c = GslVectorIterator(*this);
    ++m_i;
    return c;
  }

  double operator*() const {
    return gsl_vector_get(m_v, m_i);
  }

  double &operator*() {
    return *gsl_vector_ptr(m_v, m_i);
  }
};

// Provide a const iterator for gsl_vector
class GslVectorConstIterator: public std::iterator<std::input_iterator_tag, double> {
private:
  const gsl_vector *m_v;
  size_t m_i;

public:

  GslVectorConstIterator(const gsl_vector *v): m_v{v}, m_i{0} {}

  GslVectorConstIterator(const GslVectorConstIterator&) = default;

  GslVectorConstIterator& operator++() {
    ++m_i;
    return *this;
  }

  GslVectorConstIterator operator++(int) {
    auto c = GslVectorConstIterator(*this);
    ++m_i;
    return c;
  }

  double operator*() const {
    return gsl_vector_get(m_v, m_i);
  }
};

LeastSquareSummary GSLEngine::solveProblem(ModelFitting::EngineParameterManager& parameter_manager,
                                           ModelFitting::ResidualEstimator& residual_estimator) {
  // Create a tuple which keeps the references to the given manager and estimator
  // If we capture, we can not use the lambda for the function pointer
  auto adata = std::tie(parameter_manager, residual_estimator);

  // Only type supported by GSL
  const gsl_multifit_nlinear_type *type = gsl_multifit_nlinear_trust;

  // Initialize parameters
  gsl_multifit_nlinear_parameters params = gsl_multifit_nlinear_default_parameters();

  // Allocate the workspace for the resolver. It may return null if there is no memory.
  gsl_multifit_nlinear_workspace *workspace = gsl_multifit_nlinear_alloc(
    type, &params,
    residual_estimator.numberOfResiduals(), parameter_manager.numberOfParameters()
  );
  if (workspace == nullptr) {
    throw Elements::Exception() << "Insufficient memory for initializing the GSL solver";
  }

  // Allocate space for the parameters and initialize with the guesses
  std::vector<double> param_values (parameter_manager.numberOfParameters());
  parameter_manager.getEngineValues(param_values.begin());
  gsl_vector_view gsl_param_view = gsl_vector_view_array(param_values.data(), param_values.size());

  // Function to be minimized
  auto function = [](const gsl_vector* x, void *extra, gsl_vector *f) -> int {
    auto *extra_ptr = (decltype(adata) *) extra;
    EngineParameterManager& pm = std::get<0>(*extra_ptr);
    pm.updateEngineValues(GslVectorConstIterator{x});
    ResidualEstimator& re = std::get<1>(*extra_ptr);
    re.populateResiduals(GslVectorIterator{f});
    return GSL_SUCCESS;
  };
  gsl_multifit_nlinear_fdf fdf;
  fdf.f = function;
  fdf.df = nullptr;
  fdf.fvv = nullptr;
  fdf.n = residual_estimator.numberOfResiduals();
  fdf.p = parameter_manager.numberOfParameters();
  fdf.params = &adata;

  // Setup the solver
  gsl_multifit_nlinear_init(&gsl_param_view.vector, &fdf, workspace);

  // Initial cost
  double chisq0;
  gsl_vector *residual = gsl_multifit_nlinear_residual(workspace);
  gsl_blas_ddot(residual, residual, &chisq0);

  // Solve
  int info = 0;
  int ret = gsl_multifit_nlinear_driver(
    m_itmax,  // Maximum number of iterations
    m_xtol,   // Tolerance for the step
    m_gtol,   // Tolerance for the gradient
    m_ftol,   // Tolerance for chi^2 (may be unused)
    nullptr,  // Callback
    nullptr,  // Callback parameters
    &info,    // Convergence information if GSL_SUCCESS
    workspace // The solver workspace
  );

  // Final cost
  double chisq;
  gsl_blas_ddot(residual, residual, &chisq);

  // Build run summary
  std::vector<double> covariance_matrix (parameter_manager.numberOfParameters() * parameter_manager.numberOfParameters());

  LeastSquareSummary summary;
  summary.success_flag = (ret == GSL_SUCCESS);
  summary.iteration_no = gsl_multifit_nlinear_niter(workspace);
  summary.parameter_sigmas = {};

  // Covariance matrix. Note: Do not free J! It is owned by the workspace.
  gsl_matrix *J = gsl_multifit_nlinear_jac(workspace);
  gsl_matrix_view covar = gsl_matrix_view_array(covariance_matrix.data(), parameter_manager.numberOfParameters(),
                                                parameter_manager.numberOfParameters());
  gsl_multifit_nlinear_covar(J, 0.0, &covar.matrix);

  auto converted_covariance_matrix = parameter_manager.convertCovarianceMatrixToWorldSpace(covariance_matrix);
  for (unsigned int i=0; i<parameter_manager.numberOfParameters(); i++) {
    summary.parameter_sigmas.push_back(sqrt(converted_covariance_matrix[i*(parameter_manager.numberOfParameters()+1)]));
  }

  // Levmar-compatible engine info
  int levmar_reason = 0;
  if (ret == GSL_SUCCESS) {
    levmar_reason = (info == 1) ? 2 : 1;
  }
  else if (ret == GSL_EMAXITER) {
    levmar_reason = 3;
  }

  summary.underlying_framework_info = std::array<double, 10>{
    chisq0, // ||e||_2 at initial p
    chisq,  // ||e||_2
    0., // ||J^T e||_inf
    gsl_blas_dnrm2(workspace->dx), // ||Dp||_2
    0., // mu/max[J^T J]_ii
    static_cast<double>(summary.iteration_no), // Number of iterations
    static_cast<double>(levmar_reason), // Reason for finishing
    static_cast<double>(fdf.nevalf), // Function evaluations
    static_cast<double>(fdf.nevaldf), // Jacobian evaluations
    0. // Linear systems solved
  };

  // Free resources and return
  gsl_multifit_nlinear_free(workspace);
  return summary;
}

} // end namespace ModelFitting

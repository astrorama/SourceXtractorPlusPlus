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

#include "ModelFitting/Engine/GSLEngine.h"
#include "ModelFitting/Parameters/DependentParameter.h"
#include "ModelFitting/Parameters/NeutralConverter.h"
#include <boost/test/unit_test.hpp>
#include <array>
#include <cmath>
#include <memory>
#include <utility>
#include <vector>

using namespace ModelFitting;

namespace {

class LinearResidual : public ResidualBlockProvider {
public:
  explicit LinearResidual(std::shared_ptr<EngineParameter> parameter) : m_parameter(std::move(parameter)) {}

  size_t numberOfResiduals() const override {
    return 2;
  }

  void populateResidualBlock(IterType output_iter) override {
    *output_iter++ = m_parameter->getEngineValue() - 2.;
    *output_iter = m_parameter->getEngineValue() - 4.;
  }

private:
  std::shared_ptr<EngineParameter> m_parameter;
};

class ExponentialConverter : public CoordinateConverter {
public:
  explicit ExponentialConverter(double& derivative_value) : m_derivative_value(derivative_value) {}

  double worldToEngine(double world_value) const override {
    return std::log(world_value);
  }

  double engineToWorld(double engine_value) const override {
    return std::exp(engine_value);
  }

  double getEngineToWorldDerivative(double world_value) const override {
    m_derivative_value = world_value;
    return world_value;
  }

private:
  double& m_derivative_value;
};

struct GslEngineFixture {
  void registerParameter(std::shared_ptr<EngineParameter> parameter) {
    parameter_manager.registerParameter(parameter);
    residual_estimator.registerBlockProvider(std::make_unique<LinearResidual>(parameter));
  }

  EngineParameterManager parameter_manager;
  ResidualEstimator residual_estimator;
};

} // namespace

BOOST_AUTO_TEST_SUITE(GSLEngineTest)

// BUG-018: numerical differentiation can leave the runtime parameter at a probe near the optimum.
// Check that a successful solve restores the accepted value (3) and updates dependent parameters too.
BOOST_FIXTURE_TEST_CASE(AcceptedSolution_test, GslEngineFixture) {
  auto parameter = std::make_shared<EngineParameter>(1., std::make_unique<NeutralConverter>());
  auto dependent = createDependentParameter([](double value) { return 2. * value; }, parameter);
  registerParameter(parameter);

  auto summary = GSLEngine().solveProblem(parameter_manager, residual_estimator);

  BOOST_REQUIRE_EQUAL(summary.status_flag, LeastSquareSummary::SUCCESS);
  // Much tighter than the default finite-difference displacement (about 3e-4).
  BOOST_CHECK_SMALL(parameter->getEngineValue() - 3., 1e-7);
  BOOST_CHECK_SMALL(dependent->getValue() - 6., 2e-7);
}

// Restoration must also happen when the solver stops before convergence. The optimum is not expected
// after one iteration, so compare the runtime residual cost with GSL's cost at its accepted position.
BOOST_FIXTURE_TEST_CASE(IterationLimit_test, GslEngineFixture) {
  auto parameter = std::make_shared<EngineParameter>(1., std::make_unique<NeutralConverter>());
  registerParameter(parameter);

  auto summary = GSLEngine(1).solveProblem(parameter_manager, residual_estimator);

  BOOST_REQUIRE_EQUAL(summary.status_flag, LeastSquareSummary::MAX_ITER);
  auto info = boost::any_cast<std::array<double, 10>>(summary.underlying_framework_info);
  std::vector<double> residuals(2);
  residual_estimator.populateResiduals(residuals.begin());
  double chisq = residuals[0] * residuals[0] + residuals[1] * residuals[1];
  // The returned runtime state must reproduce the accepted point's cost, even without convergence.
  BOOST_CHECK_SMALL(chisq - info[1], 1e-10);
}

// Restoring parameters only before returning would leave covariance conversion using the probe value.
// An exponential converter makes its derivative depend on the current world value; check both the
// derivative's evaluation point and the resulting world-space uncertainty at the accepted solution.
BOOST_FIXTURE_TEST_CASE(CovarianceAtAcceptedSolution_test, GslEngineFixture) {
  double derivative_value = 0.;
  auto parameter = std::make_shared<EngineParameter>(std::exp(1.),
      std::make_unique<ExponentialConverter>(derivative_value));
  registerParameter(parameter);

  auto summary = GSLEngine().solveProblem(parameter_manager, residual_estimator);

  BOOST_REQUIRE_EQUAL(summary.status_flag, LeastSquareSummary::SUCCESS);
  BOOST_REQUIRE_EQUAL(summary.parameter_sigmas.size(), 1);
  // In engine space J=[1,1], chi-square=2, and there is one degree of freedom: sigma=1.
  BOOST_CHECK_SMALL(derivative_value - std::exp(3.), 1e-6);
  BOOST_CHECK_SMALL(summary.parameter_sigmas[0] - std::exp(3.), 1e-6);
  BOOST_CHECK_SMALL(parameter->getValue() - std::exp(3.), 1e-6);
}

BOOST_AUTO_TEST_SUITE_END()

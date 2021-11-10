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

#include "AlexandriaKernel/memory_tools.h"
#include "ModelFitting/Engine/EngineValueResidual.h"
#include "ModelFitting/Engine/LevmarEngine.h"
#include "ModelFitting/Parameters/NeutralConverter.h"
#include <boost/test/unit_test.hpp>

using namespace ModelFitting;
using Euclid::make_unique;

//-----------------------------------------------------------------------------

struct EngineTestFixture {
  EngineParameterManager parameterManager;
  ResidualEstimator      residualEstimator;
};

//-----------------------------------------------------------------------------

// A residual provider
class SquareFunctionResidual final : public ResidualBlockProvider {
public:
  SquareFunctionResidual(const std::vector<double>& data, const EngineParameterManager& parameterManager)
      : m_data(data), m_parameterManager(parameterManager) {}

  size_t numberOfResiduals() const override {
    return m_data.size();
  }

  void populateResidualBlock(IterType output_iter) override {
    std::vector<double> params(3);
    m_parameterManager.getEngineValues(params.begin());

    auto di = m_data.begin();
    for (size_t i = 0; i < m_data.size(); ++i, ++output_iter, ++di) {
      double x     = static_cast<double>(i);
      *output_iter = *di - (params[0] * x * x + params[1] * x + params[2]);
    }
  }

  ~SquareFunctionResidual() override = default;

private:
  const std::vector<double>&    m_data;
  const EngineParameterManager& m_parameterManager;
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(EngineTest)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(Basic_test, EngineTestFixture) {
  LevmarEngine engine(100);

  // We want to find a, b and c s.t. y = a * x^2 + b * x + c
  // Here are the y=2 * x**2 - 3 * x + 4 values with some noise
  std::vector<double> data{4.02376764,  3.04025667,  5.97498281,  12.86715943,  24.01711412,
                           39.06889941, 58.01360317, 81.05822195, 108.08688585, 138.93037559};

  // We have three parameters
  auto a = std::make_shared<EngineParameter>(0, make_unique<NeutralConverter>());
  auto b = std::make_shared<EngineParameter>(0, make_unique<NeutralConverter>());
  auto c = std::make_shared<EngineParameter>(0, make_unique<NeutralConverter>());

  parameterManager.registerParameter(a);
  parameterManager.registerParameter(b);
  parameterManager.registerParameter(c);

  residualEstimator.registerBlockProvider(make_unique<SquareFunctionResidual>(data, parameterManager));

  // Run the engine
  auto summary = engine.solveProblem(parameterManager, residualEstimator);

  // Verify the result
  BOOST_REQUIRE_EQUAL(summary.status_flag, LeastSquareSummary::SUCCESS);
  BOOST_CHECK_CLOSE(a->getEngineValue(), 2., 1);
  BOOST_CHECK_CLOSE(b->getEngineValue(), -3., 1);
  BOOST_CHECK_CLOSE(c->getEngineValue(), 4., 1);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(EmptyBlockProvider_test, EngineTestFixture) {
  LevmarEngine engine(100);

  // We want to find a, b and c s.t. y = a * x^2 + b * x + c
  // Here are the y=2 * x**2 - 3 * x + 4 values with some noise
  std::vector<double> data{4.02376764,  3.04025667,  5.97498281,  12.86715943,  24.01711412,
                           39.06889941, 58.01360317, 81.05822195, 108.08688585, 138.93037559};

  // We have three parameters
  auto a = std::make_shared<EngineParameter>(0, make_unique<NeutralConverter>());
  auto b = std::make_shared<EngineParameter>(0, make_unique<NeutralConverter>());
  auto c = std::make_shared<EngineParameter>(0, make_unique<NeutralConverter>());

  parameterManager.registerParameter(a);
  parameterManager.registerParameter(b);
  parameterManager.registerParameter(c);

  residualEstimator.registerBlockProvider(make_unique<SquareFunctionResidual>(data, parameterManager));

  std::vector<double> empty_data{};
  residualEstimator.registerBlockProvider(make_unique<SquareFunctionResidual>(empty_data, parameterManager));

  // Run the engine
  auto summary = engine.solveProblem(parameterManager, residualEstimator);

  // Verify the result
  BOOST_REQUIRE_EQUAL(summary.status_flag, LeastSquareSummary::SUCCESS);
  BOOST_CHECK_CLOSE(a->getEngineValue(), 2., 1);
  BOOST_CHECK_CLOSE(b->getEngineValue(), -3., 1);
  BOOST_CHECK_CLOSE(c->getEngineValue(), 4., 1);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()

//-----------------------------------------------------------------------------

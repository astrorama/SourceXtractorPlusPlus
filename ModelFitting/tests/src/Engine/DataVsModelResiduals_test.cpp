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

#include "ModelFitting/Engine/AsinhChiSquareComparator.h"
#include "ModelFitting/Engine/ChiSquareComparator.h"
#include "ModelFitting/Engine/DataVsModelResiduals.h"
#include "ModelFitting/Engine/LeastSquareEngineManager.h"
#include "ModelFitting/Engine/WorldValueResidual.h"
#include "ModelFitting/Parameters/NeutralConverter.h"
#include <boost/test/unit_test.hpp>
#include <algorithm>
#include <array>
#include <cmath>
#include <iterator>
#include <limits>
#include <list>
#include <memory>
#include <utility>
#include <vector>

using namespace ModelFitting;

namespace {

// Like FrameModel, render from the current parameter whenever iteration begins.
class ConstantModel {
public:
  using const_iterator = std::vector<double>::const_iterator;

  ConstantModel(std::shared_ptr<EngineParameter> parameter, size_t size)
      : m_parameter(std::move(parameter)), m_pixels(size) {}

  size_t size() const {
    return m_pixels.size();
  }

  const_iterator begin() {
    std::fill(m_pixels.begin(), m_pixels.end(), m_parameter->getValue());
    return m_pixels.begin();
  }

  const_iterator end() const {
    return m_pixels.end();
  }

private:
  std::shared_ptr<EngineParameter> m_parameter;
  std::vector<double> m_pixels;
};

struct SharedWeights {
  using const_iterator = std::vector<double>::const_iterator;

  size_t size() const {
    return values.size();
  }

  const_iterator begin() const {
    return values.begin();
  }

  const_iterator end() const {
    return values.end();
  }

  const std::vector<double>& values;
};

LeastSquareSummary fitWithPadding(const std::string& engine_name, size_t padding, bool with_prior,
                                 double& fitted_value, bool robust = false) {
  auto parameter = std::make_shared<EngineParameter>(0., std::make_unique<NeutralConverter>());
  EngineParameterManager manager;
  manager.registerParameter(parameter);
  ResidualEstimator estimator;
  std::vector<double> data(padding + 2, 123.);
  std::vector<double> weights(padding + 2, 0.);
  data.front() = 1.;
  data.back() = 3.;
  weights.front() = weights.back() = 1.;
  if (robust) {
    estimator.registerBlockProvider(createDataVsModelResiduals(
        data, ConstantModel(parameter, data.size()), weights, AsinhChiSquareComparator{}));
  } else {
    estimator.registerBlockProvider(createDataVsModelResiduals(
        data, ConstantModel(parameter, data.size()), weights, ChiSquareComparator{}));
  }
  if (with_prior) {
    estimator.registerBlockProvider(std::make_unique<WorldValueResidual>(parameter, 2., 1.));
  }
  auto summary = LeastSquareEngineManager::create(engine_name)->solveProblem(manager, estimator);
  fitted_value = parameter->getValue();
  return summary;
}

} // namespace

BOOST_AUTO_TEST_SUITE(DataVsModelResidualsTest)

// Different values at each pixel make mismatches between data/model/weights visible.
// A zero residual at a usable pixel must still occupy an output entry.
BOOST_AUTO_TEST_CASE(SelectedPixelAlignment_test) {
  std::list<double> data{100., 3., 9., 200., 11., 300.};
  std::list<double> model{1., 3., 5., 2., 8., 3.};
  std::list<double> weights{0., 1., 2., 0., 3., 0.};
  auto provider = createDataVsModelResiduals(data, model, weights, ChiSquareComparator{});
  BOOST_REQUIRE_EQUAL(provider->numberOfResiduals(), 3);
  std::vector<double> output(4, -999.);
  provider->populateResidualBlock(output.data());
  std::vector<double> expected{0., 8., 9., -999.};
  BOOST_CHECK_EQUAL_COLLECTIONS(output.begin(), output.end(), expected.begin(), expected.end());
}

BOOST_AUTO_TEST_CASE(ModelRefresh_test) {
  auto parameter = std::make_shared<EngineParameter>(2., std::make_unique<NeutralConverter>());
  auto provider = createDataVsModelResiduals(std::vector<double>{100., 3., 200., 7.},
      ConstantModel(parameter, 4), std::vector<double>{0., 1., 0., 2.}, ChiSquareComparator{});
  BOOST_REQUIRE_EQUAL(provider->numberOfResiduals(), 2);
  std::vector<double> output(2);
  provider->populateResidualBlock(output.data());
  BOOST_CHECK_EQUAL(output[0], 1.);
  BOOST_CHECK_EQUAL(output[1], 10.);
  parameter->setValue(3.);
  provider->populateResidualBlock(output.data());
  BOOST_CHECK_EQUAL(output[0], 0.);
  BOOST_CHECK_EQUAL(output[1], 8.);
}

BOOST_AUTO_TEST_CASE(MaskedPixelsAreNotCompared_test) {
  // Evaluating a comparator at a masked NaN/Inf would contaminate the objective.
  auto invalid = std::numeric_limits<double>::quiet_NaN();
  int calls = 0;
  auto comparator = [&calls](double data, double model, double weight) {
    ++calls;
    return weight * (data - model);
  };
  auto provider = createDataVsModelResiduals(std::vector<double>{invalid, 5., invalid},
      std::vector<double>{invalid, 2., invalid}, std::vector<double>{0., 2., 0.}, comparator);
  BOOST_REQUIRE_EQUAL(provider->numberOfResiduals(), 1);
  double output = -999.;
  provider->populateResidualBlock(&output);
  BOOST_CHECK_EQUAL(calls, 1);
  BOOST_CHECK_EQUAL(output, 6.);
}

BOOST_AUTO_TEST_CASE(EmptyAndMixedBlocks_test) {
  auto empty_block = []() {
    return createDataVsModelResiduals(std::vector<double>{1., 2.}, std::vector<double>{0., 0.},
        std::vector<double>{0., 0.}, AsinhChiSquareComparator{});
  };
  auto provider = empty_block();
  BOOST_REQUIRE_EQUAL(provider->numberOfResiduals(), 0);
  provider->populateResidualBlock(nullptr);

  ResidualEstimator estimator;
  estimator.registerBlockProvider(empty_block());
  BOOST_REQUIRE_EQUAL(estimator.numberOfResiduals(), 0);
  estimator.populateResiduals(static_cast<double*>(nullptr));
  std::vector<double> output;
  estimator.populateResiduals(output.begin());
  estimator.populateResiduals(std::back_inserter(output));
  BOOST_CHECK(output.empty());

  estimator.registerBlockProvider(createDataVsModelResiduals(std::vector<double>{3., 99., 7.},
      std::vector<double>{1., 0., 2.}, std::vector<double>{1., 0., 2.}, ChiSquareComparator{}));
  estimator.registerBlockProvider(empty_block());
  estimator.registerBlockProvider(createDataVsModelResiduals(std::vector<double>{5., 6.},
      std::vector<double>{1., 1.}, std::vector<double>{0., 3.}, ChiSquareComparator{}));
  estimator.registerBlockProvider(empty_block());
  BOOST_REQUIRE_EQUAL(estimator.numberOfResiduals(), 3);
  std::vector<double> expected{2., 10., 15.};
  output.resize(3);
  estimator.populateResiduals(output.begin());
  BOOST_CHECK_EQUAL_COLLECTIONS(output.begin(), output.end(), expected.begin(), expected.end());
  estimator.populateResiduals(output.data());
  BOOST_CHECK_EQUAL_COLLECTIONS(output.begin(), output.end(), expected.begin(), expected.end());
  output.clear();
  estimator.populateResiduals(std::back_inserter(output));
  BOOST_CHECK_EQUAL_COLLECTIONS(output.begin(), output.end(), expected.begin(), expected.end());
}

BOOST_AUTO_TEST_CASE(InputSizesAndUsableCount_test) {
  BOOST_CHECK_THROW(createDataVsModelResiduals(std::vector<double>{1.}, std::vector<double>{},
      std::vector<double>{1.}, ChiSquareComparator{}), Elements::Exception);
  BOOST_CHECK_THROW(createDataVsModelResiduals(std::vector<double>{1.}, std::vector<double>{1.},
      std::vector<double>{}, ChiSquareComparator{}), Elements::Exception);
  // Padding must not hide an insufficient-data boundary (e.g. two free parameters).
  for (size_t usable : {0, 1, 2}) {
    std::vector<double> weights(100, 0.);
    std::fill_n(weights.begin(), usable, 1.);
    auto provider = createDataVsModelResiduals(std::vector<double>(100, 1.), std::vector<double>(100, 0.),
        weights, ChiSquareComparator{});
    BOOST_CHECK_EQUAL(provider->numberOfResiduals(), usable);
  }
  auto no_data = createDataVsModelResiduals(std::vector<double>{}, std::vector<double>{},
      std::vector<double>{}, ChiSquareComparator{});
  BOOST_CHECK_EQUAL(no_data->numberOfResiduals(), 0);
}

BOOST_AUTO_TEST_CASE(ChangedMaskIsRejected_test) {
  std::vector<double> weights{1., 0.};
  auto provider = createDataVsModelResiduals(std::vector<double>{3., 4.},
      std::vector<double>{1., 2.}, SharedWeights{weights}, ChiSquareComparator{});
  std::vector<double> output{0., -999.};

  weights[1] = 1.;
  BOOST_CHECK_THROW(provider->populateResidualBlock(output.data()), Elements::Exception);
  BOOST_CHECK_EQUAL(output[1], -999.);

  weights[0] = weights[1] = 0.;
  BOOST_CHECK_THROW(provider->populateResidualBlock(output.data()), Elements::Exception);
}

BOOST_AUTO_TEST_CASE(SolverRelaysChangedMaskException_test) {
  for (const auto& engine_name : LeastSquareEngineManager::getImplementations()) {
    BOOST_TEST_CONTEXT("engine=" << engine_name) {
      auto parameter = std::make_shared<EngineParameter>(0., std::make_unique<NeutralConverter>());
      EngineParameterManager manager;
      manager.registerParameter(parameter);
      std::vector<double> weights{1., 1., 0.};
      ResidualEstimator estimator;
      estimator.registerBlockProvider(createDataVsModelResiduals(std::vector<double>{1., 3., 5.},
          ConstantModel(parameter, weights.size()), SharedWeights{weights}, ChiSquareComparator{}));
      auto engine = LeastSquareEngineManager::create(engine_name);
      weights[2] = 1.;
      BOOST_CHECK_THROW(engine->solveProblem(manager, estimator), Elements::Exception);

      weights[2] = 0.;
      BOOST_CHECK_EQUAL(engine->solveProblem(manager, estimator).status_flag, LeastSquareSummary::SUCCESS);
    }
  }
}

BOOST_AUTO_TEST_CASE(MaskedPaddingUncertainty_test) {
  auto engines = LeastSquareEngineManager::getImplementations();
  if (engines.empty()) {
    BOOST_TEST_MESSAGE("No least-square backend available; provider tests still run.");
  }
  for (const auto& engine_name : engines) {
    BOOST_TEST_CONTEXT("engine=" << engine_name) {
      for (bool with_prior : {false, true}) {
        BOOST_TEST_CONTEXT("prior=" << with_prior) {
          double reference_value, padded_value;
          auto reference = fitWithPadding(engine_name, 0, with_prior, reference_value);
          auto padded = fitWithPadding(engine_name, 98, with_prior, padded_value);
          BOOST_REQUIRE_EQUAL(reference.status_flag, LeastSquareSummary::SUCCESS);
          BOOST_REQUIRE_EQUAL(padded.status_flag, LeastSquareSummary::SUCCESS);
          BOOST_REQUIRE_EQUAL(reference.parameter_sigmas.size(), 1);
          BOOST_REQUIRE_EQUAL(padded.parameter_sigmas.size(), 1);
          BOOST_CHECK_SMALL(reference_value - 2., 1e-3);
          BOOST_CHECK_SMALL(padded_value - reference_value, 1e-8);
          auto reference_info = boost::any_cast<std::array<double, 10>>(reference.underlying_framework_info);
          auto padded_info = boost::any_cast<std::array<double, 10>>(padded.underlying_framework_info);
          BOOST_CHECK_SMALL(reference_info[1] - 2., 1e-8);
          BOOST_CHECK_SMALL(padded_info[1] - reference_info[1], 1e-8);
          BOOST_CHECK_SMALL(padded.parameter_sigmas[0] - reference.parameter_sigmas[0], 1e-6);
          double expected_sigma = with_prior ? std::sqrt(1. / 3.) : 1.;
          BOOST_CHECK_SMALL(reference.parameter_sigmas[0] - expected_sigma, 1e-6);
        }
      }
    }
  }
}

// The iterative task uses the robust comparator; its transformed objective must also be invariant.
BOOST_AUTO_TEST_CASE(RobustMaskedPadding_test) {
  for (const auto& engine_name : LeastSquareEngineManager::getImplementations()) {
    BOOST_TEST_CONTEXT("engine=" << engine_name) {
      double reference_value, padded_value;
      auto reference = fitWithPadding(engine_name, 0, false, reference_value, true);
      auto padded = fitWithPadding(engine_name, 98, false, padded_value, true);
      BOOST_REQUIRE_EQUAL(reference.status_flag, LeastSquareSummary::SUCCESS);
      BOOST_REQUIRE_EQUAL(padded.status_flag, LeastSquareSummary::SUCCESS);
      BOOST_REQUIRE_EQUAL(reference.parameter_sigmas.size(), 1);
      BOOST_REQUIRE_EQUAL(padded.parameter_sigmas.size(), 1);
      BOOST_CHECK_SMALL(padded_value - reference_value, 1e-8);
      auto reference_info = boost::any_cast<std::array<double, 10>>(reference.underlying_framework_info);
      auto padded_info = boost::any_cast<std::array<double, 10>>(padded.underlying_framework_info);
      BOOST_CHECK_SMALL(padded_info[1] - reference_info[1], 1e-8);
      BOOST_CHECK_SMALL(padded.parameter_sigmas[0] - reference.parameter_sigmas[0], 1e-6);
    }
  }
}

BOOST_AUTO_TEST_SUITE_END()

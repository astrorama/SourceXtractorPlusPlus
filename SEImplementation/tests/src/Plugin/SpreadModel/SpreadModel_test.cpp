/** Copyright © 2019-2026 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
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
#include <boost/test/unit_test.hpp>
#include <cmath>
#include <limits>
#include <numeric>

#include "SEImplementation/Plugin/SpreadModel/SpreadModelUtils.h"
#include "SEImplementation/Plugin/SpreadModel/SpreadModelTask.h"
#include "SEImplementation/Plugin/MeasurementFrameRectangle/MeasurementFrameRectangle.h"
#include "SEImplementation/Plugin/PixelCentroid/PixelCentroid.h"
#include "SEImplementation/Plugin/IsophotalFlux/IsophotalFlux.h"
#include "SEFramework/Source/SimpleSource.h"
#include "SEUtils/IsNan.h"
#include "ModelFitting/Engine/LeastSquareEngineManager.h"
#include "SEImplementation/Plugin/MeasurementFrameCoordinates/MeasurementFrameCoordinates.h"
#include "SEImplementation/Plugin/MeasurementFrameImages/MeasurementFrameImages.h"
#include "SEImplementation/Plugin/MeasurementFrameInfo/MeasurementFrameInfo.h"
#include "SEImplementation/Plugin/ReferenceCoordinates/ReferenceCoordinates.h"
#include "SEImplementation/Plugin/SourcePsf/SourcePsfProperty.h"

using namespace SourceXtractor;
namespace {
std::shared_ptr<VectorImage<SeFloat>> gaussian(int size, double sx, double sy, double amplitude = 1) {
  auto image = VectorImage<SeFloat>::create(size, size);
  for (int y = 0; y < size; ++y) {
    for (int x = 0; x < size; ++x) {
      const double dx = (x - size / 2) / sx;
      const double dy = (y - size / 2) / sy;
      image->at(x, y) = amplitude * std::exp(-(dx * dx + dy * dy) / 2);
    }
  }
  return image;
}
double sum(const VectorImage<SeFloat>& image) {
  return std::accumulate(image.getData().begin(), image.getData().end(), 0.0);
}
double moment(const VectorImage<SeFloat>& image, double cx, double cy, bool radial) {
  double result = 0;
  for (int y = 0; y < image.getHeight(); ++y) {
    for (int x = 0; x < image.getWidth(); ++x) {
      const double dx = x + 0.5 - cx;
      const double dy = y + 0.5 - cy;
      result += image.getValue(x, y) * (radial ? dx * dx + dy * dy : dx);
    }
  }
  return result / sum(image);
}
}

namespace {
class IdentityCoordinates : public CoordinateSystem {
public:
  WorldCoordinate imageToWorld(ImageCoordinate coordinate) const override {
    return {coordinate.m_x, coordinate.m_y};
  }
  ImageCoordinate worldToImage(WorldCoordinate coordinate) const override {
    return {coordinate.m_alpha, coordinate.m_delta};
  }
};

class TestSpreadEngine : public ModelFitting::LeastSquareEngine {
public:
  ModelFitting::LeastSquareSummary::StatusFlag status = ModelFitting::LeastSquareSummary::SUCCESS;
  int calls = 0;
  bool move_outside = false;

  ModelFitting::LeastSquareSummary solveProblem(ModelFitting::EngineParameterManager& manager,
                                               ModelFitting::ResidualEstimator&) override {
    ++calls;
    if (move_outside) {
      std::vector<double> parameters(manager.numberOfParameters());
      manager.getEngineValues(parameters.begin());
      parameters[0] = -10; // Move x almost five reference pixels to the left.
      manager.updateEngineValues(parameters.begin());
    }
    ModelFitting::LeastSquareSummary result;
    result.status_flag = status;
    return result;
  }
};

struct SpreadFitFixture {
  SimpleSource source;
  std::shared_ptr<TestSpreadEngine> engine = std::make_shared<TestSpreadEngine>();
  std::string engine_name = "spread-test-" + std::string(boost::unit_test::framework::current_test_case().p_name);
  SpreadModelTask task{8, {8}, 50, engine_name, 1, 1000, {{8, true}}};

  SpreadFitFixture() {
    ModelFitting::LeastSquareEngineManager::registerEngine(engine_name,
        [test_engine = engine](unsigned) { return test_engine; });
    auto coordinates = std::make_shared<IdentityCoordinates>();
    source.setProperty<PixelCentroid>(2, 2);
    source.setProperty<IsophotalFlux>(100, 1, 0, 0);
    source.setProperty<ReferenceCoordinates>(coordinates);
    source.setIndexedProperty<MeasurementFrameCoordinates>(8, coordinates);
    source.setIndexedProperty<MeasurementFrameInfo>(8, 9, 9, 0, 65000, 10, 1);
    source.setIndexedProperty<SourcePsfProperty>(8, 1, gaussian(9, 1, 1));
    // An edge-clipped stamp, smaller than the full measurement frame.
    source.setIndexedProperty<MeasurementFrameRectangle>(8, ImageCoordinate(0, 0), ImageCoordinate(5, 5));
    setValidPixels(25);
  }

  void setValidPixels(int count) {
    auto data = gaussian(9, 1, 1, 100);
    auto variance = VectorImage<SeFloat>::create(9, 9);
    for (int i = 0; i < count; ++i) {
      variance->at(i % 5, i / 5) = 1;
    }
    auto frame = std::make_shared<MeasurementImageFrame>(data, nullptr, variance);
    frame->setBackgroundLevel(0);
    source.setIndexedProperty<MeasurementFrameImages>(8, frame, 9, 9);
  }

  void checkInvalid() {
    const auto& result = source.getProperty<SpreadModel>(8);
    BOOST_CHECK(fastmath_isnan(result.getSpreadModel()));
    BOOST_CHECK(fastmath_isnan(result.getSpreadModelError()));
  }
};
}

BOOST_AUTO_TEST_SUITE(SpreadModel_test)

BOOST_AUTO_TEST_CASE(psf_width) {
  const auto circular = gaussian(65, 6, 6);
  const auto elliptical = gaussian(65, 4, 8);
  const double factor = 2 * std::sqrt(2 * std::log(2.0));
  BOOST_CHECK_CLOSE(SpreadModelUtils::computePsfFwhm(*circular, 1), factor * 6, 1);
  BOOST_CHECK_CLOSE(SpreadModelUtils::computePsfFwhm(*elliptical, 1), factor * std::sqrt(32.0), 1);
  BOOST_CHECK_CLOSE(SpreadModelUtils::computePsfFwhm(*circular, 0.25), factor * 1.5, 1);
  BOOST_CHECK_CLOSE(SpreadModelUtils::computePsfFwhm(*gaussian(65, 6, 6, 7), 1),
                    SpreadModelUtils::computePsfFwhm(*circular, 1), 0.05);
  BOOST_CHECK_CLOSE(SpreadModelUtils::computePsfFwhm(*elliptical, 1, 16),
                    SpreadModelUtils::computePsfFwhm(*elliptical, 1, 64), 0.2);
  auto multiple = gaussian(65, 3, 3);
  const auto original_width = SpreadModelUtils::computePsfFwhm(*multiple, 1);
  multiple->at(5, 5) = 0.9;
  BOOST_CHECK_EQUAL(SpreadModelUtils::computePsfFwhm(*multiple, 1), original_width);
}

BOOST_AUTO_TEST_CASE(invalid_psf) {
  auto kernel = gaussian(9, 8, 8);
  BOOST_CHECK(fastmath_isnan(SpreadModelUtils::computePsfFwhm(*kernel, 1)));
  kernel = gaussian(9, 1, 1, 0);
  BOOST_CHECK(fastmath_isnan(SpreadModelUtils::computePsfFwhm(*kernel, 1)));
  kernel = gaussian(9, 1, 1);
  kernel->at(0, 0) = std::numeric_limits<SeFloat>::quiet_NaN();
  BOOST_CHECK(fastmath_isnan(SpreadModelUtils::computePsfFwhm(*kernel, 1)));
  kernel->at(0, 0) = std::numeric_limits<SeFloat>::infinity();
  BOOST_CHECK(fastmath_isnan(SpreadModelUtils::computePsfFwhm(*kernel, 1)));
  BOOST_CHECK(fastmath_isnan(SpreadModelUtils::computePsfFwhm(*gaussian(9, 1, 1), 0)));
}

BOOST_AUTO_TEST_CASE(statistic_and_derivative) {
  auto phi = VectorImage<SeFloat>::create(3, 1, std::vector<SeFloat>{1, 4, 1});
  auto g = VectorImage<SeFloat>::create(3, 1, std::vector<SeFloat>{2, 2, 2});
  auto p = VectorImage<SeFloat>::create(3, 1, std::vector<SeFloat>{3, 5, 2});
  std::vector<double> variance{1, 2, 3};
  std::vector<bool> valid(3, true);
  auto star = SpreadModelUtils::computeSpread(*phi, *g, *phi, variance, valid);
  BOOST_CHECK_SMALL(double(star.getSpreadModel()), 1e-6);
  BOOST_CHECK_GT(SpreadModelUtils::computeSpread(*phi, *g, *g, variance, valid).getSpreadModel(), 0);
  const auto result = SpreadModelUtils::computeSpread(*phi, *g, *p, variance, valid);
  double numerical_error_squared = 0;
  for (int i = 0; i < 3; ++i) {
    const auto original = p->getValue(i, 0);
    p->at(i, 0) = original + 0.01;
    const auto plus = SpreadModelUtils::computeSpread(*phi, *g, *p, variance, valid).getSpreadModel();
    p->at(i, 0) = original - 0.01;
    const auto minus = SpreadModelUtils::computeSpread(*phi, *g, *p, variance, valid).getSpreadModel();
    p->at(i, 0) = original;
    const double derivative = (plus - minus) / 0.02;
    numerical_error_squared += derivative * derivative * variance[i];
  }
  BOOST_CHECK_CLOSE(result.getSpreadModelError(), std::sqrt(numerical_error_squared), 0.1);
  for (int i = 0; i < 3; ++i) {
    phi->at(i, 0) *= 7;
    g->at(i, 0) *= 7;
  }
  const auto scaled = SpreadModelUtils::computeSpread(*phi, *g, *p, variance, valid);
  BOOST_CHECK_CLOSE(scaled.getSpreadModel(), result.getSpreadModel(), 0.001);
  BOOST_CHECK_CLOSE(scaled.getSpreadModelError(), result.getSpreadModelError(), 0.001);
  valid[0] = false;
  const auto masked = SpreadModelUtils::computeSpread(*phi, *g, *p, variance, valid);
  p->at(0, 0) = std::numeric_limits<SeFloat>::quiet_NaN();
  const auto masked_nan = SpreadModelUtils::computeSpread(*phi, *g, *p, variance, valid);
  BOOST_CHECK_EQUAL(masked.getSpreadModel(), masked_nan.getSpreadModel());
  valid.assign(3, false);
  BOOST_CHECK(fastmath_isnan(SpreadModelUtils::computeSpread(*phi, *g, *p, variance, valid).getSpreadModel()));
  valid.assign(3, true);
  p->at(0, 0) = p->at(1, 0) = p->at(2, 0) = 0;
  BOOST_CHECK(fastmath_isnan(SpreadModelUtils::computeSpread(*phi, *g, *p, variance, valid).getSpreadModel()));
  p->at(1, 0) = -1;
  BOOST_CHECK(fastmath_isnan(SpreadModelUtils::computeSpread(*phi, *g, *p, variance, valid).getSpreadModel()));
}

BOOST_AUTO_TEST_CASE(rendering) {
  auto kernel = gaussian(65, 6, 6);
  const double total = sum(*kernel);
  for (auto& value : kernel->getData()) {
    value /= total;
  }
  const double fwhm = SpreadModelUtils::computePsfFwhm(*kernel, 0.25);
  DownSampledImagePsf psf(0.25, kernel);
  const auto stamps = SpreadModelUtils::renderModels(20.8, 20.3, 100, fwhm, 41, 41, psf);
  BOOST_CHECK_CLOSE(sum(*stamps.first), 100, 0.5);
  BOOST_CHECK_CLOSE(sum(*stamps.second), 100, 0.5);
  BOOST_CHECK_SMALL(moment(*stamps.first, 20.8, 20.3, false), 0.02);
  BOOST_CHECK_SMALL(moment(*stamps.second, 20.8, 20.3, false), 0.02);
  BOOST_CHECK_GT(moment(*stamps.second, 20.8, 20.3, true), moment(*stamps.first, 20.8, 20.3, true));
  const auto centered = SpreadModelUtils::renderModels(20.5, 20.5, 100, fwhm, 41, 41, psf);
  BOOST_CHECK_CLOSE(centered.second->getValue(21, 20), centered.second->getValue(20, 21), 0.1);
  DownSampledImagePsf coarse(0.25, kernel, 0.5);
  const auto downsampled = SpreadModelUtils::renderModels(20.8, 20.3, 100, fwhm, 41, 41, coarse);
  BOOST_CHECK_CLOSE(sum(*downsampled.second), sum(*stamps.second), 0.5);
  BOOST_CHECK_CLOSE(moment(*downsampled.second, 20.8, 20.3, true),
                    moment(*stamps.second, 20.8, 20.3, true), 5);
}

BOOST_AUTO_TEST_CASE(frame_indices_are_independent) {
  SimpleSource source;
  source.setProperty<PixelCentroid>(0, 0);
  source.setProperty<IsophotalFlux>(1, 1, 0, 0);
  source.setIndexedProperty<MeasurementFrameRectangle>(3, false);
  source.setIndexedProperty<MeasurementFrameRectangle>(8, false);
  source.setIndexedProperty<SpreadModel>(3, 0.123, 0.01);
  SpreadModelTask task(8, {3, 8}, 50, "levmar", 1, 1000, {{3, true}, {8, true}});
  task.computeProperties(source);
  BOOST_CHECK_CLOSE(source.getProperty<SpreadModel>(3).getSpreadModel(), 0.123, 0.001);
  BOOST_CHECK(fastmath_isnan(source.getProperty<SpreadModel>(8).getSpreadModel()));
}
BOOST_FIXTURE_TEST_CASE(minimum_valid_pixels, SpreadFitFixture) {
  for (int count = 0; count < 4; ++count) {
    setValidPixels(count);
    task.computeProperties(source);
    checkInvalid();
    BOOST_CHECK_EQUAL(engine->calls, 0);
  }
  setValidPixels(4);
  task.computeProperties(source);
  BOOST_CHECK_EQUAL(engine->calls, 1);
}

BOOST_FIXTURE_TEST_CASE(initial_centre_coverage, SpreadFitFixture) {
  for (const auto& position : std::vector<ImageCoordinate>{{-0.6, 2}, {8.5, 2}, {2, -0.6}, {2, 8.5}}) {
    source.setProperty<PixelCentroid>(position.m_x, position.m_y);
    task.computeProperties(source);
    checkInvalid();
  }
  BOOST_CHECK_EQUAL(engine->calls, 0);
  // Inside the full frame, including the first/last pixel and beyond the stamp.
  for (const auto& position : std::vector<ImageCoordinate>{{0, 2}, {8, 2}, {2, 0}, {2, 8}}) {
    source.setProperty<PixelCentroid>(position.m_x, position.m_y);
    task.computeProperties(source);
  }
  BOOST_CHECK_EQUAL(engine->calls, 4);
}

BOOST_FIXTURE_TEST_CASE(fitted_centre_outside, SpreadFitFixture) {
  engine->move_outside = true;
  task.computeProperties(source);
  BOOST_CHECK_EQUAL(engine->calls, 1);
  checkInvalid();
}

BOOST_FIXTURE_TEST_CASE(solver_status, SpreadFitFixture) {
  for (auto status : {ModelFitting::LeastSquareSummary::ERROR, ModelFitting::LeastSquareSummary::MEMORY}) {
    engine->status = status;
    task.computeProperties(source);
    checkInvalid();
  }
  for (auto status : {ModelFitting::LeastSquareSummary::SUCCESS, ModelFitting::LeastSquareSummary::MAX_ITER}) {
    engine->status = status;
    task.computeProperties(source);
    BOOST_CHECK(!fastmath_isnan(source.getProperty<SpreadModel>(8).getSpreadModel()));
  }
  BOOST_CHECK_EQUAL(engine->calls, 4);
}

BOOST_AUTO_TEST_SUITE_END()

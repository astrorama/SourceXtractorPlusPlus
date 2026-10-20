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

#include "SEImplementation/Plugin/SpreadModelAlt/SpreadModelAltUtils.h"
#include "SEImplementation/Plugin/SpreadModelAlt/JointSpreadModelAlt.h"
#include "SEImplementation/Plugin/SpreadModelAlt/JointSpreadModelAltTask.h"
#include "SEImplementation/Plugin/SpreadModelAlt/SpreadModelAltConfig.h"
#include "ElementsKernel/Exception.h"
#include "SEImplementation/Plugin/Jacobian/Jacobian.h"
#include "SEImplementation/Image/ImageInterfaceTraits.h"
#include "ModelFitting/Models/ExtendedModel.h"
#include "ModelFitting/Models/CompactExponentialModel.h"
#include "ModelFitting/Models/FrameModel.h"
#include "ModelFitting/Parameters/ManualParameter.h"
#include "SEImplementation/Plugin/SpreadModelAlt/SpreadModelAltTask.h"
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
  std::vector<std::size_t> parameter_counts;
  bool fail_extended = false;
  int extended_shift_parameter = -1;
  bool shift_frame_outside = false;
  std::size_t pixel_residual_count = 0;
  std::vector<double> pixel_chi_squared;

  ModelFitting::LeastSquareSummary solveProblem(ModelFitting::EngineParameterManager& manager,
                                               ModelFitting::ResidualEstimator& residuals) override {
    ++calls;
    parameter_counts.push_back(manager.numberOfParameters());
    if (move_outside || shift_frame_outside || (calls > 1 && extended_shift_parameter >= 0)) {
      std::vector<double> parameters(manager.numberOfParameters());
      manager.getEngineValues(parameters.begin());
      if (move_outside) {
        parameters[0] = -10; // Move x almost five reference pixels to the left.
      }
      if (shift_frame_outside) {
        parameters[3] = -20; // One frame: x/y, flux, offset_x, offset_y.
      }
      if (calls > 1 && extended_shift_parameter >= 0) {
        parameters[extended_shift_parameter] = 2;
        parameters[extended_shift_parameter + 1] = 2;
      }
      manager.updateEngineValues(parameters.begin());
    }
    if (pixel_residual_count > 0) {
      std::vector<double> values(residuals.numberOfResiduals());
      residuals.populateResiduals(values.begin());
      double chi_squared = 0;
      for (std::size_t i = 0; i < pixel_residual_count; ++i) {
        chi_squared += values[i] * values[i];
      }
      pixel_chi_squared.push_back(chi_squared);
    }
    ModelFitting::LeastSquareSummary result;
    result.status_flag = fail_extended && calls > 1 ? ModelFitting::LeastSquareSummary::ERROR : status;
    return result;
  }
};

struct SpreadFitFixture {
  SimpleSource source;
  std::shared_ptr<TestSpreadEngine> engine = std::make_shared<TestSpreadEngine>();
  std::string engine_name = "spread-test-" + std::string(boost::unit_test::framework::current_test_case().p_name);
  SpreadModelAltTask task{8, {8}, 50, engine_name, 1, 1000, {{8, true}}};

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
    const auto& result = source.getProperty<SpreadModelAlt>(8);
    BOOST_CHECK(fastmath_isnan(result.getSpreadModelAlt()));
    BOOST_CHECK(fastmath_isnan(result.getSpreadModelAltError()));
  }
};
}

BOOST_AUTO_TEST_SUITE(SpreadModelAlt_test)

BOOST_AUTO_TEST_CASE(psf_width) {
  const auto circular = gaussian(65, 6, 6);
  const auto elliptical = gaussian(65, 4, 8);
  const double factor = 2 * std::sqrt(2 * std::log(2.0));
  BOOST_CHECK_CLOSE(SpreadModelAltUtils::computePsfFwhm(*circular, 1), factor * 6, 1);
  BOOST_CHECK_CLOSE(SpreadModelAltUtils::computePsfFwhm(*elliptical, 1), factor * std::sqrt(32.0), 1);
  BOOST_CHECK_CLOSE(SpreadModelAltUtils::computePsfFwhm(*circular, 0.25), factor * 1.5, 1);
  BOOST_CHECK_CLOSE(SpreadModelAltUtils::computePsfFwhm(*gaussian(65, 6, 6, 7), 1),
                    SpreadModelAltUtils::computePsfFwhm(*circular, 1), 0.05);
  BOOST_CHECK_CLOSE(SpreadModelAltUtils::computePsfFwhm(*elliptical, 1, 16),
                    SpreadModelAltUtils::computePsfFwhm(*elliptical, 1, 64), 0.2);
  auto multiple = gaussian(65, 3, 3);
  const auto original_width = SpreadModelAltUtils::computePsfFwhm(*multiple, 1);
  multiple->at(5, 5) = 0.9;
  BOOST_CHECK_EQUAL(SpreadModelAltUtils::computePsfFwhm(*multiple, 1), original_width);
}

BOOST_AUTO_TEST_CASE(invalid_psf) {
  auto kernel = gaussian(9, 8, 8);
  BOOST_CHECK(fastmath_isnan(SpreadModelAltUtils::computePsfFwhm(*kernel, 1)));
  kernel = gaussian(9, 1, 1, 0);
  BOOST_CHECK(fastmath_isnan(SpreadModelAltUtils::computePsfFwhm(*kernel, 1)));
  kernel = gaussian(9, 1, 1);
  kernel->at(0, 0) = std::numeric_limits<SeFloat>::quiet_NaN();
  BOOST_CHECK(fastmath_isnan(SpreadModelAltUtils::computePsfFwhm(*kernel, 1)));
  kernel->at(0, 0) = std::numeric_limits<SeFloat>::infinity();
  BOOST_CHECK(fastmath_isnan(SpreadModelAltUtils::computePsfFwhm(*kernel, 1)));
  BOOST_CHECK(fastmath_isnan(SpreadModelAltUtils::computePsfFwhm(*gaussian(9, 1, 1), 0)));
}

BOOST_AUTO_TEST_CASE(statistic_and_derivative) {
  auto phi = VectorImage<SeFloat>::create(3, 1, std::vector<SeFloat>{1, 4, 1});
  auto g = VectorImage<SeFloat>::create(3, 1, std::vector<SeFloat>{2, 2, 2});
  auto p = VectorImage<SeFloat>::create(3, 1, std::vector<SeFloat>{3, 5, 2});
  std::vector<double> variance{1, 2, 3};
  std::vector<bool> valid(3, true);
  auto star = SpreadModelAltUtils::computeSpread(*phi, *g, *phi, variance, valid);
  BOOST_CHECK_SMALL(double(star.getSpreadModelAlt()), 1e-6);
  BOOST_CHECK_GT(SpreadModelAltUtils::computeSpread(*phi, *g, *g, variance, valid).getSpreadModelAlt(), 0);
  const auto result = SpreadModelAltUtils::computeSpread(*phi, *g, *p, variance, valid);
  double numerical_error_squared = 0;
  for (int i = 0; i < 3; ++i) {
    const auto original = p->getValue(i, 0);
    p->at(i, 0) = original + 0.01;
    const auto plus = SpreadModelAltUtils::computeSpread(*phi, *g, *p, variance, valid).getSpreadModelAlt();
    p->at(i, 0) = original - 0.01;
    const auto minus = SpreadModelAltUtils::computeSpread(*phi, *g, *p, variance, valid).getSpreadModelAlt();
    p->at(i, 0) = original;
    const double derivative = (plus - minus) / 0.02;
    numerical_error_squared += derivative * derivative * variance[i];
  }
  BOOST_CHECK_CLOSE(result.getSpreadModelAltError(), std::sqrt(numerical_error_squared), 0.1);
  for (int i = 0; i < 3; ++i) {
    phi->at(i, 0) *= 7;
    g->at(i, 0) *= 7;
  }
  const auto scaled = SpreadModelAltUtils::computeSpread(*phi, *g, *p, variance, valid);
  BOOST_CHECK_CLOSE(scaled.getSpreadModelAlt(), result.getSpreadModelAlt(), 0.001);
  BOOST_CHECK_CLOSE(scaled.getSpreadModelAltError(), result.getSpreadModelAltError(), 0.001);
  valid[0] = false;
  const auto masked = SpreadModelAltUtils::computeSpread(*phi, *g, *p, variance, valid);
  p->at(0, 0) = std::numeric_limits<SeFloat>::quiet_NaN();
  const auto masked_nan = SpreadModelAltUtils::computeSpread(*phi, *g, *p, variance, valid);
  BOOST_CHECK_EQUAL(masked.getSpreadModelAlt(), masked_nan.getSpreadModelAlt());
  valid.assign(3, false);
  BOOST_CHECK(fastmath_isnan(SpreadModelAltUtils::computeSpread(*phi, *g, *p, variance, valid).getSpreadModelAlt()));
  valid.assign(3, true);
  p->at(0, 0) = p->at(1, 0) = p->at(2, 0) = 0;
  BOOST_CHECK(fastmath_isnan(SpreadModelAltUtils::computeSpread(*phi, *g, *p, variance, valid).getSpreadModelAlt()));
  p->at(1, 0) = -1;
  BOOST_CHECK(fastmath_isnan(SpreadModelAltUtils::computeSpread(*phi, *g, *p, variance, valid).getSpreadModelAlt()));
}

BOOST_AUTO_TEST_CASE(rendering) {
  auto kernel = gaussian(65, 6, 6);
  const double total = sum(*kernel);
  for (auto& value : kernel->getData()) {
    value /= total;
  }
  const double fwhm = SpreadModelAltUtils::computePsfFwhm(*kernel, 0.25);
  DownSampledImagePsf psf(0.25, kernel);
  const auto stamps = SpreadModelAltUtils::renderModels(20.8, 20.3, 100, fwhm, 41, 41, psf);
  BOOST_CHECK_CLOSE(sum(*stamps.first), 100, 0.5);
  BOOST_CHECK_CLOSE(sum(*stamps.second), 100, 0.5);
  BOOST_CHECK_SMALL(moment(*stamps.first, 20.8, 20.3, false), 0.02);
  BOOST_CHECK_SMALL(moment(*stamps.second, 20.8, 20.3, false), 0.02);
  BOOST_CHECK_GT(moment(*stamps.second, 20.8, 20.3, true), moment(*stamps.first, 20.8, 20.3, true));
  const auto centered = SpreadModelAltUtils::renderModels(20.5, 20.5, 100, fwhm, 41, 41, psf);
  BOOST_CHECK_CLOSE(centered.second->getValue(21, 20), centered.second->getValue(20, 21), 0.1);
  DownSampledImagePsf coarse(0.25, kernel, 0.5);
  const auto downsampled = SpreadModelAltUtils::renderModels(20.8, 20.3, 100, fwhm, 41, 41, coarse);
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
  source.setIndexedProperty<SpreadModelAlt>(3, 0.123, 0.01);
  SpreadModelAltTask task(8, {3, 8}, 50, "levmar", 1, 1000, {{3, true}, {8, true}});
  task.computeProperties(source);
  BOOST_CHECK_CLOSE(source.getProperty<SpreadModelAlt>(3).getSpreadModelAlt(), 0.123, 0.001);
  BOOST_CHECK(fastmath_isnan(source.getProperty<SpreadModelAlt>(8).getSpreadModelAlt()));
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
    BOOST_CHECK(!fastmath_isnan(source.getProperty<SpreadModelAlt>(8).getSpreadModelAlt()));
  }
  BOOST_CHECK_EQUAL(engine->calls, 4);
}


BOOST_FIXTURE_TEST_CASE(joint_skips_outside_before_requesting_pixels, SpreadFitFixture) {
  // No Jacobian is installed: an outside frame must be rejected before it is needed.
  source.setProperty<PixelCentroid>(-0.6, 2);
  JointSpreadModelAltTask({8}, 50, engine_name, 1, 1000, {{8, true}}).computeProperties(source);
  const auto& result = source.getProperty<JointSpreadModelAlt>().getResult();
  BOOST_CHECK_EQUAL(result.n_frames, 0);
  BOOST_CHECK(result.flags & JointSpreadModelAlt::INSUFFICIENT_DATA);
  BOOST_CHECK(fastmath_isnan(result.radius));
  BOOST_CHECK_EQUAL(engine->calls, 0);
}

BOOST_FIXTURE_TEST_CASE(joint_two_frames_share_position_and_radius, SpreadFitFixture) {
  source.setIndexedProperty<JacobianSource>(8);
  source.setIndexedProperty<JacobianSource>(9);
  source.setIndexedProperty<MeasurementFrameCoordinates>(9, source.getProperty<MeasurementFrameCoordinates>(8));
  source.setIndexedProperty<MeasurementFrameInfo>(9, source.getProperty<MeasurementFrameInfo>(8));
  source.setIndexedProperty<MeasurementFrameRectangle>(9, source.getProperty<MeasurementFrameRectangle>(8));
  source.setIndexedProperty<MeasurementFrameImages>(9, source.getProperty<MeasurementFrameImages>(8));
  source.setIndexedProperty<SourcePsfProperty>(9, source.getProperty<SourcePsfProperty>(8));
  JointSpreadModelAltTask({8, 9}, 50, engine_name, 1, 1000, {{8, true}, {9, true}}).computeProperties(source);
  BOOST_REQUIRE_EQUAL(engine->parameter_counts.size(), 3);
  BOOST_CHECK_EQUAL(engine->parameter_counts[0], 8); // shared x/y, two fluxes, four offsets
  BOOST_CHECK_EQUAL(engine->parameter_counts[1], 9); // plus shared radius
  BOOST_CHECK_EQUAL(engine->parameter_counts[2], 9);
  BOOST_CHECK_EQUAL(source.getProperty<JointSpreadModelAlt>().getResult().n_frames, 2);
  BOOST_CHECK_EQUAL(source.getProperty<JointSpreadModelAlt>().getResult().n_pixels, 50);
}

BOOST_FIXTURE_TEST_CASE(joint_requires_residual_degrees_of_freedom, SpreadFitFixture) {
  source.setIndexedProperty<JacobianSource>(8);
  setValidPixels(4);
  JointSpreadModelAltTask({8}, 50, engine_name, 1, 1000, {{8, true}}).computeProperties(source);
  BOOST_CHECK(source.getProperty<JointSpreadModelAlt>().getResult().flags & JointSpreadModelAlt::INSUFFICIENT_DATA);
  BOOST_CHECK_EQUAL(engine->calls, 0);
}

BOOST_FIXTURE_TEST_CASE(joint_parameter_sharing_and_failures, SpreadFitFixture) {
  source.setIndexedProperty<JacobianSource>(8);
  source.setIndexedProperty<MeasurementFrameRectangle>(3, false);
  JointSpreadModelAltTask task({3, 8}, 50, engine_name, 1, 1000, {{8, true}});
  task.computeProperties(source);
  BOOST_REQUIRE_EQUAL(engine->parameter_counts.size(), 3);
  BOOST_CHECK_EQUAL(engine->parameter_counts[0], 5); // x/y, one flux, two offsets
  BOOST_CHECK_EQUAL(engine->parameter_counts[1], 6); // plus shared radius
  BOOST_CHECK_EQUAL(source.getProperty<JointSpreadModelAlt>().getResult().n_frames, 1);

  engine->calls = 0;
  engine->fail_extended = true;
  task.computeProperties(source);
  const auto& failed = source.getProperty<JointSpreadModelAlt>().getResult();
  BOOST_CHECK(failed.flags & JointSpreadModelAlt::ERROR);
  BOOST_CHECK(fastmath_isnan(failed.delta_chi_squared));

  engine->fail_extended = false;
  engine->move_outside = true;
  task.computeProperties(source);
  BOOST_CHECK(source.getProperty<JointSpreadModelAlt>().getResult().flags & JointSpreadModelAlt::OUTSIDE);
}

namespace {
class ScaledCoordinates : public CoordinateSystem {
public:
  WorldCoordinate imageToWorld(ImageCoordinate coordinate) const override {
    return {(coordinate.m_x + 2) / 1.2, (coordinate.m_y - 2) / 0.8};
  }
  ImageCoordinate worldToImage(WorldCoordinate coordinate) const override {
    return {1.2 * coordinate.m_alpha - 2, 0.8 * coordinate.m_delta + 2};
  }
};

// Generate independent frames at a known position, radius and flux. Different WCS
// axis scales make a circular reference source non-circular in the second frame.
void addJointTestFrame(SimpleSource& source, int id, double radius, double flux, double psf_width,
                       std::shared_ptr<CoordinateSystem> coordinates,
                       std::tuple<double, double, double, double> jacobian,
                       double dx = 0, double dy = 0, double variance_value = 1) {
  using namespace ModelFitting;
  using ImageType = std::shared_ptr<VectorImage<SeFloat>>;
  auto parameter = [](double value) { return std::make_shared<ManualParameter>(value); };
  auto kernel = gaussian(25, psf_width, psf_width);
  const double normalization = sum(*kernel);
  for (auto& value : kernel->getData()) {
    value /= normalization;
  }
  const auto centre = coordinates->worldToImage({10.2 + dx, 10.1 + dy});
  auto x = parameter(centre.m_x + 0.5), y = parameter(centre.m_y + 0.5), amplitude = parameter(flux);
  std::vector<PointModel> points;
  std::vector<std::shared_ptr<ExtendedModel<ImageType>>> extended;
  if (radius == 0) {
    points.emplace_back(x, y, amplitude);
  }
  else {
    extended.emplace_back(std::make_shared<CompactExponentialModel<ImageType>>(
        2.0, parameter(flux / (2 * M_PI * 0.35513 * radius * radius)), parameter(1.678 / radius),
        parameter(1), parameter(1), parameter(0), 25, 25, x, y, amplitude, jacobian));
  }
  DownSampledImagePsf psf(1, kernel);
  FrameModel<DownSampledImagePsf, ImageType> model(1, 21, 21, {}, std::move(points), std::move(extended), psf);
  auto data = model.getImage();
  auto variance = VectorImage<SeFloat>::create(21, 21);
  std::fill(variance->getData().begin(), variance->getData().end(), variance_value);
  auto frame = std::make_shared<MeasurementImageFrame>(data, nullptr, variance);
  frame->setBackgroundLevel(0);
  source.setIndexedProperty<MeasurementFrameCoordinates>(id, coordinates);
  source.setIndexedProperty<MeasurementFrameInfo>(id, 21, 21, 0, 65000, variance_value * 10, 1);
  source.setIndexedProperty<MeasurementFrameRectangle>(id, ImageCoordinate(0, 0), ImageCoordinate(21, 21));
  source.setIndexedProperty<MeasurementFrameImages>(id, frame, 21, 21);
  source.setIndexedProperty<SourcePsfProperty>(id, 1, kernel);
  source.setIndexedProperty<JacobianSource>(id, std::get<0>(jacobian), std::get<1>(jacobian),
                                             std::get<2>(jacobian), std::get<3>(jacobian));
}

JointSpreadModelAlt::Result runJointSynthetic(double radius, double sigma = 0.2, double shift = 0,
                                              double second_variance = 1) {
  SimpleSource source;
  source.setProperty<PixelCentroid>(10, 10);
  source.setProperty<IsophotalFlux>(1500, 1, 0, 0);
  source.setProperty<ReferenceCoordinates>(std::make_shared<IdentityCoordinates>());
  addJointTestFrame(source, 3, radius, 1000, 1.0, std::make_shared<IdentityCoordinates>(), {1, 0, 0, 1},
                    shift, -shift / 2);
  addJointTestFrame(source, 8, radius, 3000, 1.5, std::make_shared<ScaledCoordinates>(), {1.2, 0, 0, 0.8},
                    -shift, shift / 2, second_variance);
  JointSpreadModelAltTask({3, 8}, 300, "levmar", 1, 1000, {{3, true}, {8, true}}, sigma).computeProperties(source);
  return source.getProperty<JointSpreadModelAlt>().getResult();
}
} // namespace

BOOST_AUTO_TEST_CASE(joint_recovers_extension_across_psfs_fluxes_and_wcs) {
  const auto result = runJointSynthetic(1.2);
  BOOST_CHECK_EQUAL(result.flags, JointSpreadModelAlt::NONE);
  BOOST_CHECK_EQUAL(result.n_frames, 2);
  BOOST_CHECK_EQUAL(result.n_pixels, 882);
  BOOST_CHECK_CLOSE(result.radius, 1.2, 1);
  BOOST_CHECK_GT(result.radius_error, 0);
  BOOST_CHECK_GT(result.delta_chi_squared, 100);
  BOOST_CHECK_SMALL(result.extended_chi_squared, 0.1);
}

BOOST_AUTO_TEST_CASE(joint_unresolved_point_limit) {
  const auto result = runJointSynthetic(0);
  BOOST_CHECK_EQUAL(result.flags & (JointSpreadModelAlt::ERROR | JointSpreadModelAlt::OUTSIDE), 0);
  BOOST_CHECK(result.flags & JointSpreadModelAlt::BOUNDARY);
  BOOST_CHECK_LT(result.radius, 0.02);
  BOOST_CHECK(fastmath_isnan(result.radius_error));
  BOOST_CHECK_SMALL(result.delta_chi_squared, 0.01);
  BOOST_CHECK_SMALL(result.point_chi_squared, 0.01);
}


BOOST_AUTO_TEST_CASE(position_prior_configuration) {
  SpreadModelAltConfig config(0);
  BOOST_CHECK_EQUAL(config.getPositionPriorSigma(), 0.2);
  for (double sigma : {0.0, 0.1, 1.0}) {
    config.initialize({{"spread-model-alt-position-prior-sigma",
                        boost::program_options::variable_value(boost::any(sigma), false)}});
    BOOST_CHECK_EQUAL(config.getPositionPriorSigma(), sigma);
  }
  for (double sigma : {-1.0, std::numeric_limits<double>::quiet_NaN(),
                        std::numeric_limits<double>::infinity()}) {
    BOOST_CHECK_THROW(config.initialize({{"spread-model-alt-position-prior-sigma",
        boost::program_options::variable_value(boost::any(sigma), false)}}), Elements::Exception);
    BOOST_CHECK_THROW(JointSpreadModelAltTask({}, 50, "levmar", 1, 1000, {}, sigma), std::invalid_argument);
  }
}

BOOST_FIXTURE_TEST_CASE(joint_rejects_shifted_centre_outside, SpreadFitFixture) {
  source.setIndexedProperty<JacobianSource>(8);
  engine->shift_frame_outside = true;
  JointSpreadModelAltTask({8}, 50, engine_name, 1, 1000, {{8, true}}).computeProperties(source);
  const auto& result = source.getProperty<JointSpreadModelAlt>().getResult();
  BOOST_CHECK(result.flags & JointSpreadModelAlt::OUTSIDE);
  BOOST_CHECK(fastmath_isnan(result.delta_objective));
}

BOOST_FIXTURE_TEST_CASE(joint_selects_penalized_objective_and_preserves_frame_order, SpreadFitFixture) {
  source.setIndexedProperty<MeasurementFrameRectangle>(3, false);
  source.setIndexedProperty<JacobianSource>(8);
  engine->extended_shift_parameter = 3;
  engine->pixel_residual_count = 25;
  JointSpreadModelAltTask({3, 8}, 50, engine_name, 1, 1000, {{8, true}}, 0.001).computeProperties(source);
  const auto& result = source.getProperty<JointSpreadModelAlt>().getResult();
  BOOST_CHECK_EQUAL(result.frame_ids[0], 3);
  BOOST_CHECK_EQUAL(result.frame_ids[1], 8);
  BOOST_REQUIRE_EQUAL(result.extended_offset_x.size(), 2);
  BOOST_CHECK(fastmath_isnan(result.extended_offset_x[0]));
  BOOST_CHECK_EQUAL(result.extended_offset_x[1], 0);
  BOOST_REQUIRE_EQUAL(engine->pixel_chi_squared.size(), 3);
  BOOST_CHECK_LT(std::min(engine->pixel_chi_squared[1], engine->pixel_chi_squared[2]), engine->pixel_chi_squared[0]);
  BOOST_CHECK_EQUAL(result.radius, 0);
  BOOST_CHECK_EQUAL(result.delta_objective, 0);
  BOOST_CHECK_EQUAL(result.extended_prior_penalty, 0);
}

BOOST_AUTO_TEST_CASE(joint_zero_sigma_restores_shared_position_model) {
  const auto result = runJointSynthetic(1.2, 0);
  BOOST_CHECK_EQUAL(result.flags, JointSpreadModelAlt::NONE);
  BOOST_CHECK_CLOSE(result.radius, 1.2, 1);
  BOOST_CHECK_EQUAL(result.point_prior_penalty, 0);
  BOOST_CHECK_EQUAL(result.extended_prior_penalty, 0);
  BOOST_CHECK_EQUAL(result.point_offset_rms, 0);
  BOOST_CHECK_EQUAL(result.extended_offset_rms, 0);
  BOOST_CHECK_EQUAL(result.delta_objective, result.delta_chi_squared);
}

BOOST_AUTO_TEST_CASE(joint_recovers_frame_offsets_without_broadening_star) {
  const auto locked = runJointSynthetic(0, 0, 0.35);
  const auto shifted = runJointSynthetic(0, 0.2, 0.35);
  BOOST_CHECK_EQUAL(shifted.flags & (JointSpreadModelAlt::ERROR | JointSpreadModelAlt::OUTSIDE), 0);
  BOOST_CHECK_LT(shifted.point_chi_squared, locked.point_chi_squared * 0.1);
  // Regularization can trade a tiny residual offset against a small radius;
  // require negligible extension relative to a pixel and to the locked fit.
  BOOST_CHECK_LT(shifted.radius, 0.1);
  BOOST_CHECK_LT(shifted.radius, locked.radius * 0.5);
  BOOST_CHECK_CLOSE(shifted.point_offset_x[0] - shifted.point_offset_x[1], 0.7, 10);
  BOOST_CHECK_CLOSE(shifted.point_offset_y[0] - shifted.point_offset_y[1], -0.35, 10);
  BOOST_CHECK_SMALL(shifted.point_offset_x[0] + shifted.point_offset_x[1], 0.01);
  BOOST_CHECK_GT(shifted.point_prior_penalty, 0);
  BOOST_CHECK_GE(shifted.delta_objective, 0);
  BOOST_CHECK_SMALL(shifted.delta_objective - (shifted.delta_chi_squared + shifted.point_prior_penalty
                                            - shifted.extended_prior_penalty), 1e-6);
  double penalty = 0;
  for (std::size_t i = 0; i < shifted.point_offset_x.size(); ++i) {
    penalty += (shifted.point_offset_x[i] * shifted.point_offset_x[i]
                + shifted.point_offset_y[i] * shifted.point_offset_y[i]) / (0.2 * 0.2);
  }
  BOOST_CHECK_CLOSE(shifted.point_prior_penalty, penalty, 1e-8);
}

BOOST_AUTO_TEST_CASE(joint_prior_shrinks_weak_frame_offsets) {
  const auto regularized = runJointSynthetic(0, 0.2, 0.35, 10000);
  const auto loose = runJointSynthetic(0, 10, 0.35, 10000);
  BOOST_CHECK_EQUAL(regularized.flags & (JointSpreadModelAlt::ERROR | JointSpreadModelAlt::OUTSIDE), 0);
  BOOST_CHECK_EQUAL(loose.flags & (JointSpreadModelAlt::ERROR | JointSpreadModelAlt::OUTSIDE), 0);
  BOOST_CHECK_LT(regularized.point_offset_rms, loose.point_offset_rms * 0.5);
}

BOOST_AUTO_TEST_SUITE_END()

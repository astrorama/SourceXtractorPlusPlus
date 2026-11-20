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
#include <boost/test/unit_test.hpp>

#include <cmath>
#include <limits>
#include <numeric>

#include "SEUtils/IsNan.h"
#include "SEImplementation/Plugin/FlexibleModelFitting/FlexibleModelFitting.h"

#include "SEImplementation/Plugin/Jacobian/Jacobian.h"
#include "SEImplementation/Plugin/SourcePsf/SourcePsfProperty.h"

#include "SEFramework/Source/SimpleSource.h"
#include "SEFramework/Source/SimpleSourceGroup.h"
#include "SEImplementation/Plugin/FlexibleModelFitting/FlexibleModelFittingIterativeTask.h"
#include "SEImplementation/Plugin/FlexibleModelFitting/FlexibleModelFittingTask.h"
#include "SEImplementation/Plugin/MeasurementFrameCoordinates/MeasurementFrameCoordinates.h"
#include "SEImplementation/Plugin/MeasurementFrameGroupRectangle/MeasurementFrameGroupRectangle.h"
#include "SEImplementation/Plugin/MeasurementFrameImages/MeasurementFrameImages.h"
#include "SEImplementation/Plugin/MeasurementFrameInfo/MeasurementFrameInfo.h"
#include "SEImplementation/Plugin/MeasurementFrameRectangle/MeasurementFrameRectangle.h"
#include "SEImplementation/Plugin/ReferenceCoordinates/ReferenceCoordinates.h"
#include "SEImplementation/Plugin/WorldCentroid/WorldCentroid.h"

namespace SourceXtractor {
namespace {

class WeightTestCoordinates : public CoordinateSystem {
public:
  WorldCoordinate imageToWorld(ImageCoordinate coord) const override {
    return WorldCoordinate(coord.m_x, coord.m_y);
  }

  ImageCoordinate worldToImage(WorldCoordinate) const override {
    return ImageCoordinate(2, 2);
  }
};

}

struct FlexibleModelFittingWeightFixture {
  void checkVarianceMask() {
    const int size = 7;
    auto image = VectorImage<SeFloat>::create(size, size);
    auto variance = VectorImage<SeFloat>::create(size, size);
    image->fillValue(20);
    const SeFloat values[] = {4, 10, 100, 0, -1, std::numeric_limits<SeFloat>::infinity(),
                             std::numeric_limits<SeFloat>::quiet_NaN()};
    for (int y = 0; y < size; ++y) {
      for (int x = 0; x < size; ++x) {
        variance->at(x, y) = values[x];
      }
    }
    auto coordinates = std::make_shared<WeightTestCoordinates>();
    for (int gap : {0, 1}) {
      for (SeFloat gain : {0, 2}) {
        auto frame = std::make_shared<MeasurementImageFrame>(image, variance, 10, coordinates, gain, 0, gap);
        frame->setBackgroundLevel(0, 2);
        SimpleSource source;
        source.setIndexedProperty<MeasurementFrameImages>(0, frame, size, size);
        source.setIndexedProperty<MeasurementFrameInfo>(0, size, size, gain, 0, 10, 2);
        source.setIndexedProperty<MeasurementFrameCoordinates>(0, coordinates);
        source.setProperty<ReferenceCoordinates>(coordinates);
        source.setProperty<WorldCentroid>(2, 2);
        source.setIndexedProperty<MeasurementFrameRectangle>(0, ImageCoordinate(1, 1), ImageCoordinate(5, 5));
        FlexibleModelFittingIterativeTask task("", 1, 1, {}, {}, {}, {});
        auto weights = task.createWeightImage(source, 0);
        BOOST_REQUIRE_EQUAL(weights->getWidth(), size);
        int usable = 0;
        for (int y = 0; y < size; ++y) {
          for (int x = 0; x < size; ++x) {
            if (x == 0) {
              BOOST_CHECK_CLOSE(weights->at(x, y), 1 / std::sqrt(gain > 0 ? 14.0 : 4.0), 0.001);
            } else {
              BOOST_CHECK_EQUAL(weights->at(x, y), 0);
            }
            usable += weights->at(x, y) != 0;
          }
        }
        BOOST_CHECK_EQUAL(usable, size);
      }
    }
  }

  void checkSkippedFit(bool outside, bool masked, int free_parameters) {
    auto coordinates = std::make_shared<WeightTestCoordinates>();
    auto image = VectorImage<SeFloat>::create(5, 5);
    auto variance = VectorImage<SeFloat>::create(5, 5);
    image->fillValue(10);
    variance->fillValue(4);
    // Exercise complete masking and a nonempty stamp with too few usable pixels.
    const SeFloat saturation = masked ? 1 : 0;
    auto frame = std::make_shared<MeasurementImageFrame>(image, variance, 1e6, coordinates, 0, saturation, 0);
    frame->setBackgroundLevel(0, 2);
    auto source = std::make_unique<SimpleSource>();
    source->setIndexedProperty<MeasurementFrameImages>(0, frame, 5, 5);
    source->setIndexedProperty<MeasurementFrameInfo>(0, 5, 5, 0, saturation, 1e6, 2);
    source->setIndexedProperty<MeasurementFrameCoordinates>(0, coordinates);
    source->setProperty<ReferenceCoordinates>(coordinates);
    source->setProperty<WorldCentroid>(2, 2);
    if (outside) {
      source->setIndexedProperty<MeasurementFrameRectangle>(0, false);
    } else {
      source->setIndexedProperty<MeasurementFrameRectangle>(0, ImageCoordinate(1, 1), ImageCoordinate(3, 3));
      source->setIndexedProperty<JacobianSource>(0);
      source->setIndexedProperty<SourcePsfProperty>(0, 1.0, nullptr);
    }
    SimpleSourceGroup group;
    group.addSource(std::move(source));
    auto fitting_frame = std::make_shared<FlexibleModelFittingFrame>(0,
        std::vector<std::shared_ptr<FlexibleModelFittingModel>>{});
    FlexibleModelFittingIterativeTask task("must-not-run", 1, 1, {}, {fitting_frame}, {}, {{0, false}});
    FlexibleModelFittingIterativeTask::FittingState state;
    state.source_states.emplace_back();
    auto& result = state.source_states.back();
    result.flags = Flags::NONE;
    result.n_free_parameters = free_parameters;
    result.reduced_chi_squared = 4;
    result.parameters_sigmas[7] = 2;
    result.parameters_fitted[7] = true;
    result.parameters_values[7] = 3;
    task.fitSource(group, *group.begin(), 0, state);
    auto expected = outside ? Flags::OUTSIDE : Flags::INSUFFICIENT_DATA;
    BOOST_CHECK(result.flags == expected);
    BOOST_CHECK(fastmath_isnan(result.reduced_chi_squared));
    BOOST_CHECK(fastmath_isnan(result.parameters_sigmas.at(7)));
    BOOST_CHECK(!result.parameters_fitted.at(7));
    // Keep internal guesses for neighbor rendering; final output masks unfitted free parameters.
    BOOST_CHECK_EQUAL(result.parameters_values.at(7), 3);
  }

  void checkSparseFrameNormalization() {
    auto coordinates = std::make_shared<WeightTestCoordinates>();
    SimpleSource source;
    source.setProperty<ReferenceCoordinates>(coordinates);
    auto psf = VectorImage<SeFloat>::create(33, 33);
    psf->at(16, 16) = 2;
    for (int id : {0, 1, 7, 42}) {
      source.setIndexedProperty<MeasurementFrameCoordinates>(id, coordinates);
      source.setIndexedProperty<JacobianSource>(id);
      source.setIndexedProperty<SourcePsfProperty>(id, 1.0, psf);
    }
    auto position = std::make_shared<FlexibleModelFittingConstantParameter>(100,
        [](const SourceInterface&) { return 2.0; });
    auto flux = std::make_shared<FlexibleModelFittingConstantParameter>(101,
        [](const SourceInterface&) { return 1.0; });
    auto model = std::make_shared<FlexibleModelFittingPointModel>(position, position, flux);
    std::vector<std::shared_ptr<FlexibleModelFittingModel>> models{model};
    auto frame0 = std::make_shared<FlexibleModelFittingFrame>(0, models);
    auto frame1 = std::make_shared<FlexibleModelFittingFrame>(1, models);
    auto frame7 = std::make_shared<FlexibleModelFittingFrame>(7, models);
    auto frame42 = std::make_shared<FlexibleModelFittingFrame>(42, models);
    FlexibleModelFittingIterativeTask dense("", 1, 1, {position, flux}, {frame0, frame1}, {},
        {{0, false}, {1, true}, {99, true}});
    FlexibleModelFittingIterativeTask sparse("", 1, 1, {position, flux}, {frame7, frame42}, {},
        {{42, true}, {99, false}, {7, false}});
    FlexibleModelFittingIterativeTask::SourceState state;
    dense.fitSourcePrepareParameters(state.parameter_manager, state.engine_parameter_manager, source, state);
    auto integral = [&](const FlexibleModelFittingIterativeTask& task,
                        const std::shared_ptr<FlexibleModelFittingFrame>& frame) {
      auto frame_model = task.createFrameModel(source, 1, state.parameter_manager, frame,
          PixelRectangle(PixelCoordinate(0, 0), PixelCoordinate(32, 32)), 0.5);
      auto image = frame_model.getImage();
      return std::accumulate(image->getData().begin(), image->getData().end(), 0.0);
    };
    const auto unnormalized = integral(dense, frame0);
    const auto normalized = integral(dense, frame1);
    BOOST_REQUIRE_GT(normalized, 0.0);
    BOOST_CHECK_CLOSE(unnormalized, 2 * normalized, 0.001);
    BOOST_CHECK_CLOSE(integral(sparse, frame7), unnormalized, 0.001);
    BOOST_CHECK_CLOSE(integral(sparse, frame42), normalized, 0.001);
    // Extra loaded images without assigned models do not affect frame selection.
    FlexibleModelFittingIterativeTask subset("", 1, 1, {position, flux}, {frame7}, {},
        {{42, true}, {99, true}, {7, false}});
    BOOST_CHECK_CLOSE(integral(subset, frame7), unnormalized, 0.001);
  }

  void checkWeights(bool iterative, SeFloat saturation, SeFloat scale, bool varying_background, SeFloat gain, SeFloat background_level = 30) {
    const int size = 5;
    auto image = VectorImage<SeFloat>::create(size, size);
    auto background = VectorImage<SeFloat>::create(size, size);
    auto variance = VectorImage<SeFloat>::create(size, size);
    variance->fillValue(4 * scale * scale);

    // Below, exactly at, and above saturation; also exercise nonpositive signal.
    const SeFloat pixels[] = {99, 100, 101, 20, 10};
    for (int y = 0; y < size; ++y) {
      for (int x = 0; x < size; ++x) {
        image->at(x, y) = pixels[x] * scale;
        background->at(x, y) = (background_level + (varying_background ? y * 5 : 0)) * scale;
      }
    }

    auto coordinates = std::make_shared<WeightTestCoordinates>();
    auto frame = std::make_shared<MeasurementImageFrame>(
        image, variance, 1e6, coordinates, gain, saturation, 0);
    frame->setBackgroundLevel(background, 2 * scale);
    auto source = std::make_unique<SimpleSource>();
    source->setIndexedProperty<MeasurementFrameImages>(0, frame, size, size);
    source->setIndexedProperty<MeasurementFrameInfo>(0, size, size, gain, saturation, 1e6, 2 * scale);
    source->setIndexedProperty<MeasurementFrameCoordinates>(0, coordinates);
    source->setProperty<ReferenceCoordinates>(coordinates);
    source->setProperty<WorldCentroid>(2, 2);
    source->setIndexedProperty<MeasurementFrameRectangle>(0, ImageCoordinate(1, 1), ImageCoordinate(3, 3));

    std::shared_ptr<VectorImage<SeFloat>> weights;
    if (iterative) {
      FlexibleModelFittingIterativeTask task("", 1, 1, {}, {}, {}, {});
      weights = task.createWeightImage(*source, 0);
    } else {
      SimpleSourceGroup group;
      group.addSource(std::move(source));
      group.setIndexedProperty<MeasurementFrameGroupRectangle>(0, PixelCoordinate(0, 0), PixelCoordinate(4, 4));
      FlexibleModelFittingTask task("", 1, 1, {}, {}, {});
      weights = task.createWeightImage(group, 0);
    }

    BOOST_REQUIRE_EQUAL(weights->getWidth(), size);
    BOOST_REQUIRE_EQUAL(weights->getHeight(), size);
    for (int y = 0; y < size; ++y) {
      for (int x = 0; x < size; ++x) {
        if (saturation > 0 && x >= 1 && x <= 2) {
          BOOST_CHECK_EQUAL(weights->getValue(x, y), 0);
        } else {
          // Shot noise continues to use the background-subtracted signal.
          double signal = image->getValue(x, y) - background->getValue(x, y);
          double expected_variance = 4 * scale * scale;
          if (gain > 0 && signal > 0) {
            expected_variance += signal / gain;
          }
          BOOST_CHECK_CLOSE(weights->getValue(x, y), 1 / std::sqrt(expected_variance), 0.001);
        }
      }
    }
  }
};

BOOST_FIXTURE_TEST_SUITE(FlexibleModelFittingWeight_test, FlexibleModelFittingWeightFixture)

BOOST_AUTO_TEST_CASE(saturation_before_background_subtraction) {
  for (bool iterative : {true, false}) {
    checkWeights(iterative, 100, 1, false, 2);
    checkWeights(iterative, 100, 1, false, 2, 0);
    checkWeights(iterative, 100, 1, false, 2, -30);
    checkWeights(iterative, 100, 1, true, 2);
    checkWeights(iterative, 100, 1, true, 0);
    checkWeights(iterative, 200, 2, true, 1);
  }
}

BOOST_AUTO_TEST_CASE(disabled_saturation) {
  for (bool iterative : {true, false}) {
    checkWeights(iterative, 0, 1, true, 2);
    checkWeights(iterative, -1, 1, true, 2);
  }
}

BOOST_AUTO_TEST_CASE(skipped_fit_retains_failure_state) {
  checkSkippedFit(true, false, 1);
  checkSkippedFit(false, true, 1);
  checkSkippedFit(false, false, 26);
}

BOOST_AUTO_TEST_CASE(skipped_output_has_no_fabricated_constant_or_derived_values) {
  auto coordinates = std::make_shared<WeightTestCoordinates>();
  auto source = std::make_unique<SimpleSource>();
  source->setProperty<ReferenceCoordinates>(coordinates);
  source->setIndexedProperty<MeasurementFrameRectangle>(0, false);
  SimpleSourceGroup group;
  group.addSource(std::move(source));
  auto constant = std::make_shared<FlexibleModelFittingConstantParameter>(7,
      [](const SourceInterface&) { return 7.0; });
  auto derived = std::make_shared<FlexibleModelFittingDependentParameter>(8,
      [](const std::shared_ptr<CoordinateSystem>&, const std::vector<double>& values) { return 2 * values[0]; },
      std::vector<std::shared_ptr<FlexibleModelFittingParameter>>{constant});
  auto frame = std::make_shared<FlexibleModelFittingFrame>(0,
      std::vector<std::shared_ptr<FlexibleModelFittingModel>>{});
  FlexibleModelFittingIterativeTask task("must-not-run", 1, 1, {constant, derived}, {frame}, {}, {{0, false}});
  task.computeProperties(group);
  const auto& result = group.begin()->getProperty<FlexibleModelFitting>();
  BOOST_CHECK((result.getFlags() & Flags::OUTSIDE) != Flags::NONE);
  for (int id : {7, 8}) {
    BOOST_CHECK(fastmath_isnan(result.getParameterValue(id)));
    BOOST_CHECK(fastmath_isnan(result.getParameterSigma(id)));
  }
}

BOOST_AUTO_TEST_CASE(psf_normalization_uses_sparse_frame_ids) {
  checkSparseFrameNormalization();
}

BOOST_AUTO_TEST_CASE(finite_bad_variances_are_excluded) {
  checkVarianceMask();
}

BOOST_AUTO_TEST_SUITE_END()

}

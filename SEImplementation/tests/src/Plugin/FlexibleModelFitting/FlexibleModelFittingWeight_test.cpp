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

class StatusTestEngine : public ModelFitting::LeastSquareEngine {
public:
  explicit StatusTestEngine(ModelFitting::LeastSquareSummary::StatusFlag status) : m_status(status) {}

  ModelFitting::LeastSquareSummary solveProblem(ModelFitting::EngineParameterManager&,
                                                ModelFitting::ResidualEstimator&) override {
    ModelFitting::LeastSquareSummary summary{};
    summary.status_flag = m_status;
    summary.duration = 0;
    summary.iteration_no = 99; // A rejected workspace size must not become solver iterations.
    return summary;
  }

private:
  ModelFitting::LeastSquareSummary::StatusFlag m_status;
};

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
  void checkSolverStatus(ModelFitting::LeastSquareSummary::StatusFlag status) {
    ModelFitting::LeastSquareEngineManager::registerEngine("status-test", [status](unsigned int) {
      return std::make_shared<StatusTestEngine>(status);
    });
    auto coordinates = std::make_shared<WeightTestCoordinates>();
    auto image = VectorImage<SeFloat>::create(5, 5);
    auto variance = VectorImage<SeFloat>::create(5, 5);
    image->fillValue(10);
    variance->fillValue(4);
    auto frame = std::make_shared<MeasurementImageFrame>(image, variance, 1e6, coordinates, 0, 0, 0);
    frame->setBackgroundLevel(0, 2);
    auto source = std::make_unique<SimpleSource>();
    source->setIndexedProperty<MeasurementFrameImages>(0, frame, 5, 5);
    source->setIndexedProperty<MeasurementFrameInfo>(0, 5, 5, 0, 0, 1e6, 2);
    source->setIndexedProperty<MeasurementFrameCoordinates>(0, coordinates);
    source->setIndexedProperty<MeasurementFrameRectangle>(0, ImageCoordinate(1, 1), ImageCoordinate(3, 3));
    source->setIndexedProperty<JacobianSource>(0);
    source->setIndexedProperty<SourcePsfProperty>(0, 1.0, nullptr);
    source->setProperty<ReferenceCoordinates>(coordinates);
    source->setProperty<WorldCentroid>(2, 2);
    SimpleSourceGroup group;
    group.addSource(std::move(source));
    auto parameter = std::make_shared<FlexibleModelFittingConstantParameter>(7,
        [](const SourceInterface&) { return 7.0; });
    auto fitting_frame = std::make_shared<FlexibleModelFittingFrame>(0,
        std::vector<std::shared_ptr<FlexibleModelFittingModel>>{});
    FlexibleModelFittingIterativeTask task("status-test", 1, 1, {parameter}, {fitting_frame}, {}, {false}, 1, 1);
    task.computeProperties(group);
    const auto& result = group.begin()->getProperty<FlexibleModelFitting>();
    const bool failure = status == ModelFitting::LeastSquareSummary::MEMORY
        || status == ModelFitting::LeastSquareSummary::ERROR;
    BOOST_CHECK(((result.getFlags() & Flags::ERROR) != Flags::NONE) == failure);
    BOOST_CHECK(((result.getFlags() & Flags::MEMORY) != Flags::NONE)
        == (status == ModelFitting::LeastSquareSummary::MEMORY));
    if (failure) {
      BOOST_CHECK(fastmath_isnan(result.getParameterValue(7)));
      BOOST_CHECK(fastmath_isnan(result.getParameterSigma(7)));
      BOOST_CHECK_EQUAL(result.getIterations(), 0);
    } else {
      BOOST_CHECK_EQUAL(result.getParameterValue(7), 7);
      BOOST_CHECK_EQUAL(result.getParameterSigma(7), 0);
    }
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

BOOST_AUTO_TEST_CASE(solver_failure_does_not_publish_usable_results) {
  checkSolverStatus(ModelFitting::LeastSquareSummary::MEMORY);
  checkSolverStatus(ModelFitting::LeastSquareSummary::ERROR);
  checkSolverStatus(ModelFitting::LeastSquareSummary::SUCCESS);
  checkSolverStatus(ModelFitting::LeastSquareSummary::MAX_ITER);
}

BOOST_AUTO_TEST_SUITE_END()

}

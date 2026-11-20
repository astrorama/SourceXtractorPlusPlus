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

#include <array>
#include <cmath>

#include "SEImplementation/Plugin/Jacobian/Jacobian.h"

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
  void checkEllipseTransform() {
    auto coordinates = std::make_shared<WeightTestCoordinates>();
    SimpleSource source;
    source.setProperty<ReferenceCoordinates>(coordinates);
    source.setIndexedProperty<MeasurementFrameCoordinates>(0, coordinates);
    FlexibleModelFittingIterativeTask task("", 1, 1, {}, {}, {}, {});
    // Identity, rotation, anisotropic scaling, shear, reflection, and collapse.
    const std::vector<std::array<double, 4>> transforms = {
        {1, 0, 0, 1}, {0, 1, -1, 0}, {2, 0, 0, 0.5}, {1, 0, 1, 1}, {-1, 0, 0.4, 1}, {0, 0, 0, 0}};
    for (auto jacobian : transforms) {
      source.setIndexedProperty<JacobianSource>(0, jacobian[0], jacobian[1], jacobian[2], jacobian[3]);
      for (double angle : {0.0, 0.6}) {
        FlexibleModelFittingIterativeTask::FittingEllipse ellipse{2, 2, 2, 1, angle};
        const auto transformed = task.transformEllipse(ellipse, source, 0);
        const double c = std::cos(angle), s = std::sin(angle);
        const double u_x = 2 * (jacobian[0] * c + jacobian[2] * s);
        const double u_y = 2 * (jacobian[1] * c + jacobian[3] * s);
        const double v_x = -jacobian[0] * s + jacobian[2] * c;
        const double v_y = -jacobian[1] * s + jacobian[3] * c;
        const double tc = std::cos(transformed.m_theta), ts = std::sin(transformed.m_theta);
        const double a2 = transformed.m_a * transformed.m_a;
        const double b2 = transformed.m_b * transformed.m_b;
        // Reconstruct the shape matrix: catches both wrong axis lengths and wrong orientation.
        BOOST_CHECK_SMALL(a2 * tc * tc + b2 * ts * ts - (u_x * u_x + v_x * v_x), 1e-10);
        BOOST_CHECK_SMALL(a2 * ts * ts + b2 * tc * tc - (u_y * u_y + v_y * v_y), 1e-10);
        BOOST_CHECK_SMALL((a2 - b2) * tc * ts - (u_x * u_y + v_x * v_y), 1e-10);
        BOOST_CHECK_SMALL(transformed.m_a * transformed.m_b
            - 2 * std::abs(jacobian[0] * jacobian[3] - jacobian[1] * jacobian[2]), 1e-10);
      }
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

BOOST_AUTO_TEST_CASE(rotated_ellipse_transform_preserves_shape_and_area) {
  checkEllipseTransform();
}

BOOST_AUTO_TEST_SUITE_END()

}

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
#include <limits>

#include "SEFramework/Image/VectorImage.h"
#include "SEFramework/Source/SimpleSource.h"
#include "SEImplementation/Plugin/DetectionFrameCoordinates/DetectionFrameCoordinates.h"
#include "SEImplementation/Plugin/DetectionFrameImages/DetectionFrameImages.h"
#include "SEImplementation/Plugin/MeasurementFrameCoordinates/MeasurementFrameCoordinates.h"
#include "SEImplementation/Plugin/MeasurementFrameImages/MeasurementFrameImages.h"
#include "SEImplementation/Plugin/MeasurementFrameInfo/MeasurementFrameInfo.h"
#include "SEImplementation/Plugin/MeasurementFramePixelCentroid/MeasurementFramePixelCentroid.h"
#include "SEImplementation/Plugin/Vignet/Vignet.h"
#include "SEImplementation/Plugin/Vignet/VignetSourceTask.h"
#include "SEImplementation/Property/PixelCoordinateList.h"

using namespace SourceXtractor;

namespace {

class OffsetCoordinateSystem : public CoordinateSystem {
public:
  OffsetCoordinateSystem(double x_offset, double y_offset) : m_x_offset(x_offset), m_y_offset(y_offset) {}

  WorldCoordinate imageToWorld(ImageCoordinate coord) const override {
    return {coord.m_x - m_x_offset, coord.m_y - m_y_offset};
  }

  ImageCoordinate worldToImage(WorldCoordinate coord) const override {
    return {coord.m_alpha + m_x_offset, coord.m_delta + m_y_offset};
  }

private:
  double m_x_offset;
  double m_y_offset;
};

struct VignetFixture {
  SimpleSource source;
  static constexpr SeFloat masked_value = -999;

  VignetFixture() {
    auto measurement = VectorImage<SeFloat>::create(5, 5, std::vector<SeFloat>(25, 10));
    auto variance = VectorImage<SeFloat>::create(5, 5, std::vector<SeFloat>(25, 1));
    variance->setValue(0, 0, 100);
    auto frame = std::make_shared<MeasurementImageFrame>(measurement, nullptr, variance);
    source.setIndexedProperty<MeasurementFrameImages>(0, frame, 5, 5);
    source.setIndexedProperty<MeasurementFrameInfo>(0, 5, 5, 1, 1000, 50, 1);
    source.setIndexedProperty<MeasurementFramePixelCentroid>(0, 2, 2);
    source.setIndexedProperty<MeasurementFrameCoordinates>(0, std::make_shared<OffsetCoordinateSystem>(0, 0));

    auto detection = VectorImage<SeFloat>::create(3, 3, std::vector<SeFloat>(9, 0));
    detection->setValue(0, 0, 5); // Neighbor: masked in the vignette.
    detection->setValue(1, 1, 5); // Source's own pixel: retained.
    source.setProperty<DetectionFrameImages>(std::make_shared<DetectionImageFrame>(detection), 3, 3);
    source.setProperty<PixelCoordinateList>(PixelCoordinateList{{{1, 1}}});
    source.setProperty<DetectionFrameCoordinates>(std::make_shared<OffsetCoordinateSystem>(-1, -1));
  }
};

} // namespace

BOOST_AUTO_TEST_SUITE(VignetSourceTask_test)

BOOST_FIXTURE_TEST_CASE(partial_detection_coverage, VignetFixture) {
  VignetSourceTask(0, {7, 7}, masked_value).computeProperties(source);
  const auto& vignet = source.getProperty<Vignet>(0).getVignet();
  for (int y = 0; y < 7; ++y) {
    for (int x = 0; x < 7; ++x) {
      bool masked = x == 0 || y == 0 || x == 6 || y == 6 || (x == 1 && y == 1) || (x == 2 && y == 2);
      BOOST_CHECK_EQUAL(vignet.getValue(x, y), masked ? masked_value : 10);
    }
  }
}

BOOST_FIXTURE_TEST_CASE(invalid_detection_coordinates, VignetFixture) {
  const double invalid[] = {-1e30, 1e30, std::numeric_limits<double>::infinity(),
                            std::numeric_limits<double>::quiet_NaN()};
  for (double coord : invalid) {
    source.setProperty<DetectionFrameCoordinates>(std::make_shared<OffsetCoordinateSystem>(coord, coord));
    VignetSourceTask(0, {5, 5}, masked_value).computeProperties(source);
    const auto& vignet = source.getProperty<Vignet>(0).getVignet();
    for (int y = 0; y < 5; ++y) {
      for (int x = 0; x < 5; ++x) {
        BOOST_CHECK_EQUAL(vignet.getValue(x, y), x == 0 && y == 0 ? masked_value : 10);
      }
    }
  }
}

BOOST_AUTO_TEST_SUITE_END()

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

#include <cmath>
#include <vector>

#include <boost/test/unit_test.hpp>

#include "SEFramework/Source/SimpleSource.h"
#include "SEImplementation/Plugin/DetectionFramePixelValues/DetectionFramePixelValues.h"
#include "SEImplementation/Plugin/PeakValue/PeakValue.h"
#include "SEImplementation/Plugin/PixelCentroid/PixelCentroid.h"
#include "SEImplementation/Plugin/ShapeParameters/ShapeParameters.h"
#include "SEImplementation/Plugin/ShapeParameters/ShapeParametersTask.h"
#include "SEImplementation/Property/PixelCoordinateList.h"

using namespace SourceXtractor;

namespace {

SeFloat measureAngle(const std::vector<PixelCoordinate>& coordinates) {
  constexpr int center_x = 1;
  constexpr int center_y = 1;
  constexpr DetectionImage::PixelType pixel_intensity = 1;

  SimpleSource source;
  source.setProperty<PixelCoordinateList>(coordinates);
  source.setProperty<PixelCentroid>(center_x, center_y);
  source.setProperty<PeakValue>(pixel_intensity, pixel_intensity, center_x, center_y);
  source.setProperty<DetectionFramePixelValues>(
      std::vector<DetectionImage::PixelType>{},
      std::vector<DetectionImage::PixelType>(coordinates.size(), pixel_intensity),
      std::vector<WeightImage::PixelType>(coordinates.size(), 1));

  ShapeParametersTask{}.computeProperties(source);
  return source.getProperty<ShapeParameters>().getEllipseTheta();
}

}  // namespace

BOOST_AUTO_TEST_SUITE(ShapeParameters_test)

BOOST_AUTO_TEST_CASE(descending_diagonal) {
  BOOST_CHECK_SMALL(measureAngle({{0, 2}, {1, 1}, {2, 0}}) + M_PI / 4.0, 1e-6);
}

BOOST_AUTO_TEST_CASE(ascending_diagonal) {
  BOOST_CHECK_SMALL(measureAngle({{0, 0}, {1, 1}, {2, 2}}) - M_PI / 4.0, 1e-6);
}

BOOST_AUTO_TEST_CASE(circular_moments) {
  BOOST_CHECK_SMALL(measureAngle({{0, 1}, {2, 1}, {1, 0}, {1, 2}}) - M_PI / 4.0, 1e-6);
}

BOOST_AUTO_TEST_SUITE_END()

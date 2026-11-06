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
#include "SEImplementation/Plugin/ErrorEllipse/ErrorEllipse.h"
#include "SEImplementation/Plugin/ErrorEllipse/ErrorEllipseTask.h"
#include "SEImplementation/Plugin/PixelCentroid/PixelCentroid.h"
#include "SEImplementation/Plugin/ShapeParameters/ShapeParameters.h"
#include "SEImplementation/Property/PixelCoordinateList.h"

using namespace SourceXtractor;

namespace {

SeFloat measureAngle(const std::vector<PixelCoordinate>& coordinates) {
  SimpleSource source;
  source.setProperty<PixelCoordinateList>(coordinates);
  source.setProperty<PixelCentroid>(2, 2);
  source.setProperty<DetectionFramePixelValues>(
      std::vector<DetectionImage::PixelType>{},
      std::vector<DetectionImage::PixelType>(coordinates.size(), 1),
      std::vector<WeightImage::PixelType>(coordinates.size(), 1));
  source.setProperty<ShapeParameters>(0, 0, 0, 0, 0, 0, 0, coordinates.size(), coordinates.size(), false);

  ErrorEllipseTask{}.computeProperties(source);
  return source.getProperty<ErrorEllipse>().getEllipseThetaError();
}

void checkAxisAngle(int dx, int dy) {
  BOOST_TEST_CONTEXT("Major axis parallel to (" << dx << ", " << dy << ")") {
    auto angle = measureAngle({{2 - dx, 2 - dy}, {2 + dx, 2 + dy}});
    auto expected = std::atan2(static_cast<double>(dy), static_cast<double>(dx));
    // Reversing an axis gives the same ellipse.
    BOOST_CHECK_SMALL(std::remainder(angle - expected, M_PI), 1e-6);
  }
}

}  // namespace

BOOST_AUTO_TEST_SUITE(ErrorEllipse_test)

BOOST_AUTO_TEST_CASE(axis_aligned) {
  checkAxisAngle(1, 0);
  checkAxisAngle(0, 1);
}

BOOST_AUTO_TEST_CASE(tilted) {
  checkAxisAngle(2, 1);
  checkAxisAngle(2, -1);
  checkAxisAngle(1, 2);
  checkAxisAngle(1, -2);
}

BOOST_AUTO_TEST_CASE(equal_variances) {
  checkAxisAngle(1, 1);
  checkAxisAngle(1, -1);
  BOOST_CHECK_EQUAL(measureAngle({{1, 2}, {3, 2}, {2, 1}, {2, 3}}), 0.0);
}

BOOST_AUTO_TEST_SUITE_END()

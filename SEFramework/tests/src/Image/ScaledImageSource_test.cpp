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
#include "SEFramework/Image/ScaledImageSource.h"
#include "SEFramework/Image/BufferedImage.h"
#include "SEUtils/TestUtils.h"
#include "MathUtils/function/FunctionAdapter.h"
#include "MathUtils/numericalDifferentiation/FiniteDifference.h"

using namespace SourceXtractor;
using Euclid::MathUtils::FunctionAdapter;
using Euclid::MathUtils::derivative2nd;

struct ScaledImageSourceFixture {
  std::shared_ptr<VectorImage<SeFloat>> image3x3 = VectorImage<SeFloat>::create(
    3, 3, std::vector<SeFloat>{
      1, 4, 3,
      2, 3, 2,
      3, 4, 5,
    }
  );

  std::shared_ptr<VectorImage<SeFloat>> image1x1 = VectorImage<SeFloat>::create(
    1, 1, std::vector<SeFloat>{42}
  );
};

//--------------------------------------he---------------------------------------

BOOST_AUTO_TEST_SUITE(ScaledImageSource_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(scaled3xBilinear, ScaledImageSourceFixture) {
/**
 * Visually, it is easy to see that each 3x3 tile corresponds to one original pixel,
 * and, thus, the original value must be in the middle.
 * It can be *almost* verified as follows:
>> import numpy as np
>> from PIL import Image
>> img = np.array([1,4,3,2,3,2,3,4,5], dtype=float).reshape(3,-1)
>> scaled = np.array(Image.fromarray(img).resize((9,9), Image.BILINEAR))
 * The difference is that PIL will *not* extrapolate the boundary pixels, and instead just replicate the
 * closest value
 */

  std::shared_ptr<VectorImage<SeFloat>> expected = VectorImage<SeFloat>::create(
    9, 9, std::vector<SeFloat>{
      -0.55555556, 0.66666667, 1.88888889, 3.11111111, 4.33333333, 4., 3.66666667, 3.33333333, 3.,
      0., 1., 2., 3., 4., 3.66666667, 3.33333333, 3., 2.66666667,
      0.55555556, 1.33333333, 2.11111111, 2.88888889, 3.66666667, 3.33333333, 3., 2.66666667, 2.33333333,
      1.11111111, 1.66666667, 2.22222222, 2.77777778, 3.33333333, 3., 2.66666667, 2.33333333, 2.,
      1.66666667, 2., 2.33333333, 2.66666667, 3., 2.66666667, 2.33333333, 2., 1.66666667,
      2., 2.33333333, 2.66666667, 3., 3.33333333, 3.22222222, 3.11111111, 3., 2.88888889,
      2.33333333, 2.66666667, 3., 3.33333333, 3.66666667, 3.77777778, 3.88888889, 4., 4.11111111,
      2.66666667, 3., 3.33333333, 3.66666667, 4., 4.33333333, 4.66666667, 5., 5.33333333,
      3., 3.33333333, 3.66666667, 4., 4.33333333, 4.88888889, 5.44444444, 6., 6.55555556
    }
  );

  auto scaled_src = std::make_shared<ScaledImageSource<SeFloat>>(
    image3x3, expected->getWidth(), expected->getHeight(), ScaledImageSource<SeFloat>::InterpolationType::BILINEAR
  );
  auto scaled = BufferedImage<SeFloat>::create(scaled_src);
  BOOST_CHECK(compareImages(expected, scaled));
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(scaled3xBicubic, ScaledImageSourceFixture) {
  auto scaled_src = std::make_shared<ScaledImageSource<SeFloat>>(
    image3x3, 9, 9, ScaledImageSource<SeFloat>::InterpolationType::BICUBIC
  );
  auto scaled = BufferedImage<SeFloat>::create(scaled_src);

  // The values at the center of each 3x3 cell must match what we had
  for (int y = 0; y < 3; ++y) {
    for (int x = 0; x < 3; ++x) {
      BOOST_CHECK_CLOSE(image3x3->getValue(x, y), scaled->getValue(1 + x * 3, 1 + y * 3), 1e-7);
    }
  }
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(scaled1pxBilinear, ScaledImageSourceFixture) {
  std::shared_ptr<VectorImage<SeFloat>> expected = VectorImage<SeFloat>::create(
    3, 3, std::vector<SeFloat>(9, 42)
  );

  auto scaled_src = std::make_shared<ScaledImageSource<SeFloat>>(
    image1x1, expected->getWidth(), expected->getHeight(), ScaledImageSource<SeFloat>::InterpolationType::BILINEAR
  );
  auto scaled = BufferedImage<SeFloat>::create(scaled_src);
  BOOST_CHECK(compareImages(expected, scaled));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()

//-----------------------------------------------------------------------------

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
#include "SEImplementation/Background/SE/MedianFilter.h"
#include "SEFramework/Image/VectorImage.h"
#include "SEUtils/TestUtils.h"

using namespace SourceXtractor;

struct MedianFilterImageFixture {
  std::shared_ptr<VectorImage<SeFloat>> image = VectorImage<SeFloat>::create(
    5, 5, std::vector<SeFloat>{
      1, 2, 3, 4, 5,
      2, 2, 6, 3, 8,
      9, 2, 1, 3, 4,
      1, 3, 4, 5, 6,
      7, 6, 3, 2, 1
    }
  );

  std::shared_ptr<VectorImage<SeFloat>> variance = VectorImage<SeFloat>::create(
    5, 5, std::vector<SeFloat>{
      1.5, 1.0, 1.1, 1.3, 2.0,
      0.8, 1.1, 2.0, 1.2, 0.9,
      0.9, 1.0, 1.6, 1.5, 1.2,
      2.0, 1.0, 1.3, 1.4, 1.7,
      1.2, 1.6, 1.4, 1.3, 1.2
    });

  // Note that this threshold will force to use the median of the variance even if
  // the median for a given cell and its previous value match
  SeFloat threshold = 0.0;
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(MedianFilterImage_test)

//-----------------------------------------------------------------------------
// Median with a box size of 1 is like doing nothing
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(median1x1, MedianFilterImageFixture) {
  auto filtered = MedianFilter<SeFloat>(1, 1)(*image, *variance, threshold);
  BOOST_CHECK(compareImages(image, filtered.first));
  BOOST_CHECK(compareImages(variance, filtered.second));
}

//-----------------------------------------------------------------------------
// Median with a box size of 3x3
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(median3x3, MedianFilterImageFixture) {
  const auto expected = VectorImage<SeFloat>::create(5, 5, std::vector<SeFloat>{
    1, 2, 3, 4, 5,
    2, 2, 3, 4, 5,
    2, 2, 3, 4, 6,
    7, 3, 3, 3, 4,
    7, 6, 3, 2, 1
  });
  const auto expected_var = VectorImage<SeFloat>::create(5, 5, std::vector<SeFloat>{
    1.5, 1.1, 1.1, 1.3, 2.0,
    0.9, 1.1, 1.2, 1.3, 1.2,
    0.9, 1.1, 1.3, 1.4, 1.2,
    1.2 ,1.3, 1.4, 1.4, 1.2,
    1.2, 1.4, 1.4, 1.3, 1.2
  });

  auto filtered = MedianFilter<SeFloat>(3, 3)(*image, *variance, threshold);
  BOOST_CHECK(compareImages(expected, filtered.first));
  BOOST_CHECK(compareImages(expected_var, filtered.second));
}

//-----------------------------------------------------------------------------
// Box with 3 pixels wide, 1 pixel tall
//----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(median3x1, MedianFilterImageFixture) {
  const auto expected = VectorImage<SeFloat>::create(5, 5, std::vector<SeFloat>{
    1, 2, 3, 4, 5,
    2, 2, 3, 6, 8,
    9, 2, 2, 3, 4,
    1, 3, 4, 5, 6,
    7, 6, 3, 2, 1
  });
  const auto expected_var = VectorImage<SeFloat>::create(5, 5, std::vector<SeFloat>{
    1.5, 1.1, 1.1, 1.3, 2.0,
    0.8, 1.1, 1.2, 1.2, 0.9,
    0.9, 1.0, 1.5, 1.5, 1.2,
    2.0, 1.3, 1.3, 1.4, 1.7,
    1.2, 1.4, 1.4, 1.3, 1.2
  });
  auto filtered = MedianFilter<SeFloat>(3, 1)(*image, *variance, threshold);
  BOOST_CHECK(compareImages(expected, filtered.first));
  BOOST_CHECK(compareImages(expected_var, filtered.second));
}

//-----------------------------------------------------------------------------
// Box with 1 pixel wide, 3 pixels tall
//----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(median1x3, MedianFilterImageFixture) {
  const auto expected = VectorImage<SeFloat>::create(5, 5, std::vector<SeFloat>{
    1, 2, 3, 4, 5,
    2, 2, 3, 3, 5,
    2, 2, 4, 3, 6,
    7, 3, 3, 3, 4,
    7, 6, 3, 2, 1
  });
  const auto expected_var = VectorImage<SeFloat>::create(5, 5, std::vector<SeFloat>{
    1.5, 1.0, 1.1, 1.3, 2.0,
    0.9, 1.0, 1.6, 1.3, 1.2,
    0.9, 1.0, 1.6, 1.4, 1.2,
    1.2, 1.0, 1.4, 1.4, 1.2,
    1.2, 1.6, 1.4, 1.3, 1.2
  });
  auto filtered = MedianFilter<SeFloat>(1, 3)(*image, *variance, threshold);
  BOOST_CHECK(compareImages(expected, filtered.first));
  BOOST_CHECK(compareImages(expected_var, filtered.second));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()

//-----------------------------------------------------------------------------

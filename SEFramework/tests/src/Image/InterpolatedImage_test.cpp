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
/**
 * @file tests/src/InterpolatedImage_test.cpp
 * @date 21/06/19
 * @author Alejandro Alvarez Ayllon
 */

#include <boost/test/unit_test.hpp>
#include "SEFramework/Image/ConstantImage.h"
#include "SEFramework/Image/InterpolatedImage.h"
#include "SEFramework/Image/VectorImage.h"
#include "SEUtils/TestUtils.h"

using namespace SExtractor;

struct InterpolatedImageSourceFixture {
  std::shared_ptr<VectorImage<SeFloat>> m_image;

  InterpolatedImageSourceFixture() {
    m_image = VectorImage<SeFloat>::create(4, 4, std::vector<SeFloat>{
      11., 12., 13., 14.,
      21., 22., 23., 24.,
      31., 32., 33., 34.,
      41., 42., 43., 44.
    });
  }
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (InterpolatedImage_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE (InterpolatedImage_all_good_test, InterpolatedImageSourceFixture) {
  auto interpolated = InterpolatedImage<SeFloat>::create(m_image, ConstantImage<SeFloat>::create(4, 4, 1.), 10., 1);
  BOOST_CHECK(compareImages(interpolated, m_image));
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE (InterpolatedImage_bad_column_test, InterpolatedImageSourceFixture) {
  auto variance = VectorImage<SeFloat>::create(4, 4, std::vector<SeFloat>{
    1., 1., 5., 1.,
    1., 1., 5., 1.,
    1., 1., 5., 1.,
    1., 1., 5., 1.,
  });
  auto interpolated = InterpolatedImage<SeFloat>::create(m_image, variance, 2., 1);
  auto expected = VectorImage<SeFloat>::create(4, 4, std::vector<SeFloat>{
    11., 12., 12., 14.,
    21., 22., 22., 24.,
    31., 32., 32., 34.,
    41., 42., 42., 44.
  });
  BOOST_CHECK(compareImages(interpolated, expected));
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE (InterpolatedImage_bad_first_column_test, InterpolatedImageSourceFixture) {
  auto variance = VectorImage<SeFloat>::create(4, 4, std::vector<SeFloat>{
    5., 1., 1., 1.,
    5., 1., 1., 1.,
    5., 1., 1., 1.,
    5., 1., 1., 1.,
  });
  auto interpolated = InterpolatedImage<SeFloat>::create(m_image, variance, 2., 1);
  auto expected = VectorImage<SeFloat>::create(4, 4, std::vector<SeFloat>{
    11., 12., 13., 14.,
    21., 22., 23., 24.,
    31., 32., 33., 34.,
    41., 42., 43., 44.
  });
  BOOST_CHECK(compareImages(interpolated, expected));
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE (InterpolatedImage_bad_wide_column_test, InterpolatedImageSourceFixture) {
  auto variance = VectorImage<SeFloat>::create(4, 4, std::vector<SeFloat>{
    1., 5., 5., 1.,
    1., 5., 5., 1.,
    1., 5., 5., 1.,
    1., 5., 5., 1.,
  });
  auto interpolated = InterpolatedImage<SeFloat>::create(m_image, variance, 2., 2);
  auto expected = VectorImage<SeFloat>::create(4, 4, std::vector<SeFloat>{
    11., 11., 11., 14.,
    21., 21., 21., 24.,
    31., 31., 31., 34.,
    41., 41., 41., 44.
  });
  BOOST_CHECK(compareImages(interpolated, expected));
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE (InterpolatedImage_bad_row_test, InterpolatedImageSourceFixture) {
  auto variance = VectorImage<SeFloat>::create(4, 4, std::vector<SeFloat>{
    1., 1., 1., 1.,
    1., 1., 1., 1.,
    5., 5., 5., 5.,
    1., 1., 1., 1.,
  });
  auto interpolated = InterpolatedImage<SeFloat>::create(m_image, variance, 2., 1);
  auto expected = VectorImage<SeFloat>::create(4, 4, std::vector<SeFloat>{
    11., 12., 13., 14.,
    21., 22., 23., 24.,
    21., 22., 23., 24.,
    41., 42., 43., 44.
  });
  BOOST_CHECK(compareImages(interpolated, expected));
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE (InterpolatedImage_bad_first_row_test, InterpolatedImageSourceFixture) {
  auto variance = VectorImage<SeFloat>::create(4, 4, std::vector<SeFloat>{
    5., 5., 5., 5.,
    1., 1., 1., 1.,
    1., 1., 1., 1.,
    1., 1., 1., 1.,
  });
  auto interpolated = InterpolatedImage<SeFloat>::create(m_image, variance, 2., 1);
  auto expected = VectorImage<SeFloat>::create(4, 4, std::vector<SeFloat>{
    11., 12., 13., 14.,
    21., 22., 23., 24.,
    31., 32., 33., 34.,
    41., 42., 43., 44.
  });
  BOOST_CHECK(compareImages(interpolated, expected));
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE (InterpolatedImage_bad_wide_row_test, InterpolatedImageSourceFixture) {
  auto variance = VectorImage<SeFloat>::create(4, 4, std::vector<SeFloat>{
    1., 1., 1., 1.,
    5., 5., 5., 5.,
    5., 5., 5., 5.,
    1., 1., 1., 1.,
  });
  auto interpolated = InterpolatedImage<SeFloat>::create(m_image, variance, 2., 2);
  auto expected = VectorImage<SeFloat>::create(4, 4, std::vector<SeFloat>{
    11., 12., 13., 14.,
    11., 12., 13., 14.,
    11., 12., 13., 14.,
    41., 42., 43., 44.
  });
  BOOST_CHECK(compareImages(interpolated, expected));
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE (InterpolatedImage_bad_no_gap_test, InterpolatedImageSourceFixture) {
  auto variance = VectorImage<SeFloat>::create(4, 4, std::vector<SeFloat>{
    1., 1., 1., 1.,
    1., 1., 5., 1.,
    1., 1., 5., 1.,
    1., 1., 1., 1.,
  });
  auto interpolated = InterpolatedImage<SeFloat>::create(m_image, variance, 2., 0);
  auto expected = VectorImage<SeFloat>::create(4, 4, std::vector<SeFloat>{
    11., 12., 13., 14.,
    21., 22., 23., 24.,
    31., 32., 33., 34.,
    41., 42., 43., 44.
  });
  BOOST_CHECK(compareImages(interpolated, expected));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()

//-----------------------------------------------------------------------------

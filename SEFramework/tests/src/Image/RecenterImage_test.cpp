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
 * @file tests/src/RecenterImage_test.cpp
 * @date 11/09/18
 * @author Alejandro Alvarez Ayllon
 */

#include <boost/test/unit_test.hpp>
#include "SEFramework/Image/MirrorImage.h"
#include "SEFramework/Image/PaddedImage.h"
#include "SEFramework/Image/RecenterImage.h"
#include "SEFramework/Image/VectorImage.h"

#include "SEUtils/TestUtils.h"

using namespace SourceXtractor;

struct RecenterImage_Fixture {
  std::shared_ptr<VectorImage<SeFloat>> img;

  RecenterImage_Fixture() : img{VectorImage<SeFloat>::create(
    3, 3,
    std::vector<SeFloat>{
      1, 2, 3,
      4, 5, 6,
      7, 8, 9
    })} {
  }
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (RecenterImage_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE (RecenterImage_test, RecenterImage_Fixture) {
  auto recentered = RecenterImage<SeFloat>::create(img, PixelCoordinate{1, 1});

  auto expected = VectorImage<SeFloat>::create(3, 3, std::vector<SeFloat>{
    5, 6, 4,
    8, 9, 7,
    2, 3, 1
  });

  BOOST_CHECK(compareImages(expected, recentered));
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE (RecenterChained_test, RecenterImage_Fixture) {
  auto mirrored = MirrorImage<SeFloat>::create(img);
  auto padded = PaddedImage<SeFloat>::create(mirrored, 5, 5);
  auto recenter = RecenterImage<SeFloat>::create(padded, PixelCoordinate{2, 2});

  auto expected = VectorImage<SeFloat>::create(5, 5, std::vector<SeFloat>{
    5, 4, 0, 0, 6,
    2, 1, 0, 0, 3,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    8, 7, 0, 0, 9,
  });

  BOOST_CHECK(compareImages(expected, recenter));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()

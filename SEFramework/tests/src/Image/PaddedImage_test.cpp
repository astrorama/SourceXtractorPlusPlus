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
/*
 * @file tests/src/Image/PaddedImage_test.cpp
 * @date 10/09/18
 * @author Alejandro Alvarez Ayllon
 */

#include <boost/test/unit_test.hpp>
#include "SEFramework/Image/PaddedImage.h"
#include "SEFramework/Image/VectorImage.h"

#include "SEUtils/TestUtils.h"

using namespace SExtractor;

struct PaddedImage_Fixture {
  std::shared_ptr<VectorImage<SeFloat>> img;

  PaddedImage_Fixture() : img{VectorImage<SeFloat>::create(
    3, 3,
    std::vector<SeFloat>{
      1, 2, 3,
      4, 5, 6,
      7, 8, 9
    })} {
  }
};


BOOST_AUTO_TEST_SUITE (PaddedImage_test)

// iiii|01234|iiii
BOOST_FIXTURE_TEST_CASE ( Constant_test, PaddedImage_Fixture ) {
  auto padded = PaddedImage<SeFloat>::create(img, 5, 5);

  auto expected = VectorImage<SeFloat>::create(5, 5, std::vector<SeFloat>{
    0, 0, 0, 0, 0,
    0, 1, 2, 3, 0,
    0, 4 ,5, 6, 0,
    0, 7, 8, 9, 0,
    0, 0, 0, 0, 0,
  });

  BOOST_CHECK(compareImages(expected, padded));
}

// 0000|01234|4444
BOOST_FIXTURE_TEST_CASE ( Replicate_test, PaddedImage_Fixture ) {
  auto padded = PaddedImage<SeFloat, ReplicateCoordinates>::create(img, 7, 7);

  auto expected = VectorImage<SeFloat>::create(7, 7, std::vector<SeFloat>{
    1, 1, 1, 2, 3, 3, 3,
    1, 1, 1, 2, 3, 3, 3,
    1, 1, 1, 2, 3, 3, 3,
    4, 4, 4 ,5, 6, 6, 6,
    7, 7, 7, 8, 9, 9, 9,
    7, 7, 7, 8, 9, 9, 9,
    7, 7, 7, 8, 9, 9, 9,
  });

  BOOST_CHECK(compareImages(expected, padded));
}

// 3210|01234|4321
BOOST_FIXTURE_TEST_CASE ( Reflect_test, PaddedImage_Fixture ) {
  auto padded = PaddedImage<SeFloat, ReflectCoordinates>::create(img, 7, 7);

  auto expected = VectorImage<SeFloat>::create(7, 7, std::vector<SeFloat>{
    5, 4, 4, 5, 6, 6, 5,
    2, 1, 1, 2, 3, 3, 2,
    2, 1, 1, 2, 3, 3, 2,
    5, 4, 4 ,5, 6, 6, 5,
    8, 7, 7, 8, 9, 9, 8,
    8, 7, 7, 8, 9, 9, 8,
    5, 4, 4, 5, 6, 6, 5
  });

  BOOST_CHECK(compareImages(expected, padded));
}

// 4321|01234|3210
BOOST_FIXTURE_TEST_CASE ( Reflect101_test, PaddedImage_Fixture ) {
  auto padded = PaddedImage<SeFloat, Reflect101Coordinates>::create(img, 5, 5);

  auto expected = VectorImage<SeFloat>::create(5, 5, std::vector<SeFloat>{
    5, 4, 5, 6, 5,
    2, 1, 2, 3, 2,
    5, 4 ,5, 6, 5,
    8, 7, 8, 9, 8,
    5, 4, 5, 6, 5,
  });

  BOOST_CHECK(compareImages(expected, padded));
}

// 1234|01234|0123
BOOST_FIXTURE_TEST_CASE ( Wrap_test, PaddedImage_Fixture ) {
  auto padded = PaddedImage<SeFloat, WrapCoordinates>::create(img, 5, 5);

  auto expected = VectorImage<SeFloat>::create(5, 5, std::vector<SeFloat>{
    9, 7, 8, 9, 7,
    3, 1, 2, 3, 1,
    6, 4 ,5, 6, 4,
    9, 7, 8, 9, 7,
    3, 1, 2, 3, 1,
  });

  BOOST_CHECK(compareImages(expected, padded));
}

BOOST_AUTO_TEST_SUITE_END()


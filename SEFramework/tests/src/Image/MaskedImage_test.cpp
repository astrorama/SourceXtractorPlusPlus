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
 * @file tests/src/MaskedImafge_test.cpp
 * @date 13/02/2020
 * @author Alejandro Alvarez Ayllon
 */

#include <boost/test/unit_test.hpp>

#include "SEFramework/Image/MaskedImage.h"
#include "SEFramework/Image/VectorImage.h"

#include "SEUtils/TestUtils.h"

using namespace SourceXtractor;

struct MaskedImageFixture {
  std::shared_ptr<VectorImage<SeFloat>> img;
  std::shared_ptr<VectorImage<char>> mask;

  MaskedImageFixture() : img{
    VectorImage<SeFloat>::create(3, 3,
                                 std::vector<SeFloat>{
                                   1, 2, 3,
                                   4, 5, 6,
                                   7, 8, 9
                                 })
  }, mask{
    VectorImage<char>::create(3, 3,
                              std::vector<char>{
                                0x0, 0x0, 0x1,
                                0x0, 0x1, 0x0,
                                0x0, 0x2, 0x3
                              })
  } {
  }

};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(MaskedImage_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(maskBitAnd, MaskedImageFixture) {
  auto expected = VectorImage<SeFloat>::create(
    3, 3,
    std::vector<SeFloat>{
      1,    2, 1024,
      4, 1024,    6,
      7,    8, 1024
    });

  auto masked = MaskedImage<SeFloat, char>::create(img, mask, 1024, 0x01);
  BOOST_CHECK(compareImages(expected, masked));
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(maskEqual, MaskedImageFixture) {
  auto expected = VectorImage<SeFloat>::create(
    3, 3,
    std::vector<SeFloat>{
      1,    2, 1024,
      4, 1024,    6,
      7,    8,    9
    });

  auto masked = MaskedImage<SeFloat, char, std::equal_to<char>>::create(img, mask, 1024, 0x01);
  BOOST_CHECK(compareImages(expected, masked));
}


//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(maskNotEqual, MaskedImageFixture) {
  auto expected = VectorImage<SeFloat>::create(
    3, 3,
    std::vector<SeFloat>{
      1,    2, 1024,
      4, 1024,    6,
      7, 1024, 1024
    });

  auto masked = MaskedImage<SeFloat, char, std::not_equal_to<char>>::create(img, mask, 1024, 0x00);
  BOOST_CHECK(compareImages(expected, masked));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()

//-----------------------------------------------------------------------------

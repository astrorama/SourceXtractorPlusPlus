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
 * @file tests/src/VectorImage_test.cpp
 * @date 06/14/16
 * @author nikoapos
 */

#include <boost/test/unit_test.hpp>
#include <numeric>
#include <valarray>

#include "SEFramework/Image/VectorImage.h"

using namespace SourceXtractor;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(VectorImage_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(example_test) {

  auto image = VectorImage<int>::create(20, 30);

  image->setValue(7, 10, 99);
  image->setValue(2, 15, 33);
  image->at(5, 6) = 42;

  BOOST_CHECK(image->getValue(6, 10) == 0);
  BOOST_CHECK(image->getValue(7, 10) == 99);
  BOOST_CHECK(image->getValue(2, 15) == 33);
  BOOST_CHECK(image->getValue(5, 6) == 42);

  BOOST_CHECK(*(image->begin() + 207) == 99);
  BOOST_CHECK(*(image->begin() + 302) == 33);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(from_iterators) {
  std::valarray<int> base{0, 1, 2, 3, 4, 5, 6, 7, 8};
  auto               image = VectorImage<int>::create(5, 1, std::begin(base) + 2, std::begin(base) + 7);

  BOOST_CHECK_EQUAL(image->at(0, 0), 2);
  BOOST_CHECK_EQUAL(image->at(1, 0), 3);
  BOOST_CHECK_EQUAL(image->at(2, 0), 4);
  BOOST_CHECK_EQUAL(image->at(3, 0), 5);
  BOOST_CHECK_EQUAL(image->at(4, 0), 6);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(iterator) {
  std::vector<int> base{0, 1, 2, 3, 4, 5, 6, 7, 8};
  std::list<int>   x;
  auto             image = VectorImage<int>::create(3, 3, base);

  // Iteration
  BOOST_CHECK_EQUAL_COLLECTIONS(base.begin(), base.end(), image->begin(), image->end());

  // Iteration on const
  const auto const_image = image;
  BOOST_CHECK_EQUAL_COLLECTIONS(image->begin(), image->end(), const_image->begin(), const_image->end());

  // Comparision and addition
  BOOST_CHECK(image->begin() < image->end());
  BOOST_CHECK(image->begin() < image->begin() + 2);
  BOOST_CHECK(image->begin() <= image->begin());
  BOOST_CHECK(image->begin() >= image->begin());

  // Random access
  auto a = image->begin();
  BOOST_CHECK_EQUAL(*a, base[0]);
  BOOST_CHECK_EQUAL(*(++a), base[1]);
  BOOST_CHECK_EQUAL(*a++, base[1]);
  BOOST_CHECK_EQUAL(*a, base[2]);
  BOOST_CHECK_EQUAL(*a + 3, base[5]);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(chunk_iterators) {
  std::vector<int> base(100);
  std::iota(base.begin(), base.end(), 0);
  auto image = VectorImage<int>::create(10, 10, base);

  BOOST_CHECK(!image->mayBeShared());

  auto chunk    = image->getChunk(3, 4, 5, 5);
  auto expected = VectorImage<int>::create(
      5, 5, std::vector<int>{43, 44, 45, 46, 47, 53, 54, 55, 56, 57, 63, 64, 65, 66, 67, 73, 74, 75, 76, 77, 83, 84, 85, 86, 87});

  BOOST_CHECK(image->mayBeShared());
  BOOST_CHECK(chunk->mayBeShared());

  BOOST_CHECK_EQUAL_COLLECTIONS(expected->begin(), expected->end(), chunk->begin(), chunk->end());

  auto a = chunk->begin();
  a += 10;
  BOOST_CHECK_EQUAL(*a, 63);
  BOOST_CHECK_EQUAL(*(--a), 57);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(chunk_inplace) {
  std::vector<int> base(100);
  std::iota(base.begin(), base.end(), 0);
  auto image = VectorImage<int>::create(10, 10, base);

  auto chunk = VectorImage<int>::create(5, 5);
  image->getChunk(3, 4, *chunk);

  auto expected = VectorImage<int>::create(
      5, 5, std::vector<int>{43, 44, 45, 46, 47, 53, 54, 55, 56, 57, 63, 64, 65, 66, 67, 73, 74, 75, 76, 77, 83, 84, 85, 86, 87});

  BOOST_CHECK_EQUAL_COLLECTIONS(expected->begin(), expected->end(), chunk->begin(), chunk->end());
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()

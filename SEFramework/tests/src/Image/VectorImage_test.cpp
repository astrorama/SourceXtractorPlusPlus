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
#include <valarray>

#include "SEFramework/Image/VectorImage.h"

using namespace SourceXtractor;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (VectorImage_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( example_test ) {

  auto image = VectorImage<int>::create(20, 30);

  image->setValue(7, 10, 99);
  image->setValue(2, 15, 33);

  BOOST_CHECK(image->getValue(6, 10) == 0);
  BOOST_CHECK(image->getValue(7, 10) == 99);
  BOOST_CHECK(image->getValue(2, 15) == 33);

  BOOST_CHECK(image->getData()[207] == 99);
  BOOST_CHECK(image->getData()[302] == 33);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( from_iterators ) {
  std::valarray<int> base{0, 1, 2, 3, 4, 5, 6, 7, 8};
  auto image = VectorImage<int>::create(5, 1, std::begin(base) + 2, std::begin(base) + 7);

  BOOST_CHECK_EQUAL(image->at(0, 0), 2);
  BOOST_CHECK_EQUAL(image->at(1, 0), 3);
  BOOST_CHECK_EQUAL(image->at(2, 0), 4);
  BOOST_CHECK_EQUAL(image->at(3, 0), 5);
  BOOST_CHECK_EQUAL(image->at(4, 0), 6);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()



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
 * ImageChunk_test.cpp
 *
 *  Created on: Sep 6, 2017
 *      Author: mschefer
 */

#include <boost/test/unit_test.hpp>

#include "SEFramework/Image/VectorImage.h"
#include "SEFramework/Image/ImageChunk.h"

using namespace SourceXtractor;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (ImageChunk_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( example_test ) {

  auto image = VectorImage<int>::create(20, 30);

  image->setValue(7, 10, 99);
  image->setValue(4, 9, 33);

  auto chunk = image->getChunk(3, 2, 10, 12);

  BOOST_CHECK_EQUAL(chunk->getWidth(), 10);
  BOOST_CHECK_EQUAL(chunk->getHeight(), 12);

  BOOST_CHECK_EQUAL(chunk->getValue(7, 10), 0);
  BOOST_CHECK_EQUAL(chunk->getValue(7 - 3, 10 - 2), 99);
  BOOST_CHECK_EQUAL(chunk->getValue(7, 10), 0);
  BOOST_CHECK_EQUAL(chunk->getValue(4 - 3, 9 - 2), 33);

  auto chunk_chunk = chunk->getChunk(3, 2, 4, 7);

  BOOST_CHECK_EQUAL(chunk_chunk->getWidth(), 4);
  BOOST_CHECK_EQUAL(chunk_chunk->getHeight(), 7);

  BOOST_CHECK_EQUAL(chunk_chunk->getValue(7 - 6, 10 - 4), 99);

}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()

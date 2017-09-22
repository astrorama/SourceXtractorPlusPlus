/*
 * ImageChunk_test.cpp
 *
 *  Created on: Sep 6, 2017
 *      Author: mschefer
 */

#include <boost/test/unit_test.hpp>

#include "SEFramework/Image/VectorImage.h"
#include "SEFramework/Image/ImageChunk.h"

using namespace SExtractor;

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

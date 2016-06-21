/**
 * @file tests/src/PixelCoordinate_test.cpp
 * @date 06/07/16
 * @author mschefer
 */

#include <boost/test/unit_test.hpp>

#include "SEUtils/PixelCoordinate.h"

using namespace SExtractor;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (PixelCoordinate_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( example_test ) {
  PixelCoordinate pc(1, 3);

  BOOST_CHECK_EQUAL(1, pc.m_x);
  BOOST_CHECK_EQUAL(3, pc.m_y);

  BOOST_CHECK(pc == PixelCoordinate(1, 3));
  BOOST_CHECK(pc != PixelCoordinate(3, 1));
  BOOST_CHECK(pc != PixelCoordinate(1, 6));
  BOOST_CHECK(pc != PixelCoordinate(3, 3));

  BOOST_CHECK(pc + PixelCoordinate(5, 9) == PixelCoordinate(6, 12));
  BOOST_CHECK(pc - PixelCoordinate(5, 2) == PixelCoordinate(-4, 1));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()

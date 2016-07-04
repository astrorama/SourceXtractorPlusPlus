/**
 * @file tests/src/VectorImage_test.cpp
 * @date 06/14/16
 * @author nikoapos
 */

#include <boost/test/unit_test.hpp>

#include "SEFramework/Image/VectorImage.h"

using namespace SExtractor;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (VectorImage_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( example_test ) {

  VectorImage<int> image(20, 30);

  image.setValue(7, 10, 99);
  image.setValue(2, 15, 33);

  BOOST_CHECK(image.getValue(6, 10) == 0);
  BOOST_CHECK(image.getValue(7, 10) == 99);
  BOOST_CHECK(image.getValue(2, 15) == 33);

  BOOST_CHECK(image.getData()[207] == 99);
  BOOST_CHECK(image.getData()[302] == 33);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()



/**
 * @file tests/src/SourceGroup_test.cpp
 * @date 08/04/16
 * @author nikoapos
 */

#include <boost/test/unit_test.hpp>

#include "SEFramework/Source/SourceGroup.h"
#include <iostream>

using namespace SExtractor;

struct DummyProperty : public Property {
  int i;
  DummyProperty(int i) : i(i) {} 
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (SourceGroup_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( example_test ) {

  BOOST_FAIL("!!!! Please implement your tests !!!!");
  
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()



/**
 * @file ManualParameter_test.cpp
 *
 * Created on: January 9, 2015
 *     Author: Pierre Dubath
 */

#include <boost/test/unit_test.hpp>
#include <boost/test/test_tools.hpp>
#include "ElementsKernel/Exception.h"
// class under test
#include "ModelFitting/Parameters/ManualParameter.h"

using namespace ModelFitting;
using namespace std;

// tolerance value to compare floating point numbers
double tolerance = 1e-12;

BOOST_AUTO_TEST_SUITE (ManualParameter_test)

BOOST_AUTO_TEST_CASE(WithoutFixture) {
  ManualParameter mp {3.0};
  BOOST_CHECK_EQUAL(3.0, mp.getValue());
}

BOOST_AUTO_TEST_SUITE_END ()


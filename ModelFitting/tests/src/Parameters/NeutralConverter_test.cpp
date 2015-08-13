/**
 * @file NeutralConverter_test.cpp
 *
 * Created on: January 9, 2015
 *     Author: Pierre Dubath
 */

#include <boost/test/unit_test.hpp>
#include <boost/test/test_tools.hpp>
#include "ElementsKernel/Exception.h"
// class under test
#include "ModelFitting/Parameters/NeutralConverter.h"

using namespace ModelFitting;
using namespace std;

// tolerance value to compare floating point numbers
double tolerance = 1e-12;

BOOST_AUTO_TEST_SUITE (NeutralConverter_test)

BOOST_AUTO_TEST_CASE(WithoutFixture) {
  NeutralConverter nc {};
  BOOST_CHECK_EQUAL(3.123, nc.engineToWorld(3.123));
  BOOST_CHECK_EQUAL(17.2736, nc.worldToEngine(17.2736));
}

BOOST_AUTO_TEST_SUITE_END ()


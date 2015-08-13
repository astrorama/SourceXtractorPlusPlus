/**
 * @file SigmoidConverter_test.cpp
 *
 * Created on: January 9, 2015
 *     Author: Pierre Dubath
 */

#include <boost/test/unit_test.hpp>
#include <boost/test/test_tools.hpp>
#include "ElementsKernel/Exception.h"
// class under test
#include "ModelFitting/Parameters/SigmoidConverter.h"

using namespace ModelFitting;
using namespace std;

// tolerance value to compare floating point numbers
double tolerance = 1e-12;
// minimum and maximum parameter value in world coordinates
double min_world_value {3.0};
double max_world_value {8.0};


BOOST_AUTO_TEST_SUITE (SigmoidConverter_test)

BOOST_AUTO_TEST_CASE(WithoutFixture) {
  SigmoidConverter sc {min_world_value, max_world_value};
  //
  double expected_result_from_python {3.089931049810458};
  BOOST_CHECK_CLOSE(expected_result_from_python, sc.engineToWorld(-4.0), tolerance);
  //
  expected_result_from_python = -0.40546510810816444;
  BOOST_CHECK_CLOSE(expected_result_from_python, sc.worldToEngine(5.0), tolerance);
}

BOOST_AUTO_TEST_SUITE_END ()


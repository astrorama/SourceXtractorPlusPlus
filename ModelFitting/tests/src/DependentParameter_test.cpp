/**
 * @file DependentParameter_test.cpp
 *
 * @date Jul 30, 2015
 * @author Nikolaos Apostolakos, Pierre Dubath
 */


#include <boost/test/unit_test.hpp>
#include "ModelFitting/Parameters/ManualParameter.h"
#include "ModelFitting/Parameters/DependentParameter.h"

using namespace ModelFitting;
using namespace std;

BOOST_AUTO_TEST_SUITE (DependentParameter_test)

BOOST_AUTO_TEST_CASE(getInputValue_test) {
  ManualParameter param1 {4.0};
  ManualParameter param2 {2.0};

//  auto dep = createDependentParameter(
//        [](const array<double, 2>& arr){return arr[0]+arr[1];},
//        param1, param2
//  );
//
//  auto values = dep.getInputParameterValues();
//
//  BOOST_CHECK_EQUAL(4.0, values.at(0));
//  BOOST_CHECK_EQUAL(2.0, values.at(1));
//
//  BOOST_CHECK_EQUAL(6.0, dep.getValue());
}

BOOST_AUTO_TEST_SUITE_END ()

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
  ManualParameter param1 { 4.0 };
  ManualParameter param2 { 2.0 };

  double test_observer { 0.0 };

  param1.addObserver([&](double v) {test_observer = v;});

  BOOST_CHECK_EQUAL(0.0, test_observer);

  param1.setValue(10.0);

  BOOST_CHECK_EQUAL(10.0, test_observer);

  auto calculator = [](double mp1, double mp2) {return mp1+mp2;};

  auto dp = ModelFitting::createDependentParameter(calculator, param1, param2);

  test_observer = 0.0;

  dp.addObserver([&](double v){test_observer = v;});

  param2.setValue(7.);

  BOOST_CHECK_EQUAL(17.0, test_observer);

}

BOOST_AUTO_TEST_CASE(copyBaseClass_test) {
  ManualParameter param1 { 4.0 };
  ManualParameter param2 { 2.0 };

  param1.setValue(10.0);

  auto calculator = [](double mp1, double mp2) {return mp1+mp2;};

  auto dp = ModelFitting::createDependentParameter(calculator, param1, param2);
  ModelFitting::BasicParameter bp {dp};

  double test_observer = 0.0;

  bp.addObserver([&](double v){test_observer = v;});

  param2.setValue(7.);

  BOOST_CHECK_EQUAL(17.0, test_observer);
  BOOST_CHECK_EQUAL(17.0, bp.getValue());
}

BOOST_AUTO_TEST_CASE(copyBaseClassGet_test) {
  ManualParameter param1 { 4.0 };
  ManualParameter param2 { 2.0 };

  auto calculator = [](double mp1, double mp2) {return mp1+mp2;};

  auto dp = ModelFitting::createDependentParameter(calculator, param1, param2);
  ModelFitting::BasicParameter bp {dp};

  param1.setValue(10.0);
  param2.setValue(7.);

  BOOST_CHECK_EQUAL(17.0, bp.getValue());
}

BOOST_AUTO_TEST_SUITE_END ()

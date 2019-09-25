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

BOOST_AUTO_TEST_CASE(dependOnDependent_test) {
  ManualParameter param1 { 4.0 };
  ManualParameter param2 { 2.0 };

  auto calculator = [](double mp1, double mp2) {return mp1+mp2;};

  auto dp = ModelFitting::createDependentParameter(calculator, param1, param2);
  auto ddp = ModelFitting::createDependentParameter(calculator, dp, dp);

  param1.setValue(10.0);
  param2.setValue(7.);

  BOOST_CHECK_EQUAL(17.0, dp.getValue());
  BOOST_CHECK_EQUAL(34.0, ddp.getValue());
}

BOOST_AUTO_TEST_CASE(observeDependOnDependent_test) {
  ManualParameter param1 { 4.0 };
  ManualParameter param2 { 2.0 };

  auto calculator = [](double mp1, double mp2) {return mp1+mp2;};

  auto dp = ModelFitting::createDependentParameter(calculator, param1, param2);
  auto ddp = ModelFitting::createDependentParameter(calculator, dp, dp);

  double test_observer = 0.0;
  ddp.addObserver([&](double v){test_observer = v;});

  param1.setValue(10.0);
  param2.setValue(7.);

  BOOST_CHECK_EQUAL(34.0, test_observer);
}

BOOST_AUTO_TEST_SUITE_END ()

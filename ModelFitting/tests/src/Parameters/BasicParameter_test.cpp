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
 * @file BasicParameter_test.cpp
 *
 * Created on: August 3rd, 2015
 *     Author: Pierre Dubath
 */

#include <memory>
#include <boost/test/unit_test.hpp>
#include <boost/test/test_tools.hpp>
//
#include "ElementsKernel/Exception.h"
// class under test
#include "ModelFitting/Parameters/BasicParameter.h"

using namespace ModelFitting;
using namespace std;

// tolerance value to compare floating point numbers
double tolerance = 1e-12;

/*
 * Fixture to compare the test result against reference values
 */
struct BasicParameterFixture {

  double world_min_value {2.0};
  double world_max_value {8.0};
  double initial_parameter_value {5.0};


  BasicParameterFixture() {
    //call constructor if needed
  }

  ~BasicParameterFixture() {
    // delete fixture object if needed
  }
};

BOOST_AUTO_TEST_SUITE (BasicParameter_test)

BOOST_FIXTURE_TEST_CASE(getValue_test, BasicParameterFixture) {
  BOOST_CHECK_EQUAL(initial_parameter_value, engine_parameter.getValue());
}

BOOST_FIXTURE_TEST_CASE(getEngineValue_test, BasicParameterFixture) {
  double expected_python_value {1.949539694713897};
  BOOST_CHECK_EQUAL(expected_python_value, engine_parameter.getEngineValue());
}

BOOST_FIXTURE_TEST_CASE(getValueAfterUpdate_test, BasicParameterFixture) {
  double new_engine_value {2.3984};
  engine_parameter.updateEngineValue(new_engine_value);
  double expected_python_value {7.127570618794628};
  BOOST_CHECK_EQUAL(expected_python_value, engine_parameter.getValue());
}

BOOST_AUTO_TEST_SUITE_END ()


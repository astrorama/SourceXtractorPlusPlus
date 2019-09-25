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
 * @file EngineParameter_test.cpp
 *
 * Created on: July 29, 2015
 *     Author: Pierre Dubath
 */

#include <memory>
#include <boost/test/unit_test.hpp>
#include <boost/test/test_tools.hpp>
//
#include "ElementsKernel/Exception.h"
//
#include "ModelFitting/Parameters/CoordinateConverter.h"
#include "ModelFitting/Parameters/ExpSigmoidConverter.h"
// class under test
#include "ModelFitting/Parameters/EngineParameter.h"

using namespace ModelFitting;
using namespace std;

// tolerance value to compare floating point numbers
double tolerance = 1e-12;

/*
 * Fixture to compare the test result against reference values
 */
struct EngineParameterFixture {

  double world_min_value {2.0};
  double world_max_value {8.0};
  double initial_parameter_value {5.0};

  std::unique_ptr<CoordinateConverter> converter_ptr {new ExpSigmoidConverter{world_min_value, world_max_value} };

  EngineParameter engine_parameter = EngineParameter{initial_parameter_value, move(converter_ptr) };

  EngineParameterFixture() {
    //call constructor if needed
  }

  ~EngineParameterFixture() {
    // delete fixture object if needed
  }
};

BOOST_AUTO_TEST_SUITE (EngineParameter_test)

BOOST_FIXTURE_TEST_CASE(getValue_test, EngineParameterFixture) {
  BOOST_CHECK_EQUAL(initial_parameter_value, engine_parameter.getValue());
}

BOOST_FIXTURE_TEST_CASE(getEngineValue_test, EngineParameterFixture) {
  double expected_python_value {0.6675932907176532};
  BOOST_CHECK_EQUAL(expected_python_value, engine_parameter.getEngineValue());
}

BOOST_FIXTURE_TEST_CASE(getValueAfterUpdate_test, EngineParameterFixture) {
  const double new_engine_value {2.3984};
  engine_parameter.setEngineValue(new_engine_value);
  double expected_python_value {7.127570618794628};
  BOOST_CHECK_EQUAL(expected_python_value, engine_parameter.getValue());
}

BOOST_AUTO_TEST_SUITE_END ()


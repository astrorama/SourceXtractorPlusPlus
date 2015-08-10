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
  double expected_python_value {1.949539694713897};
  BOOST_CHECK_EQUAL(expected_python_value, engine_parameter.getEngineValue());
}

BOOST_FIXTURE_TEST_CASE(getValueAfterUpdate_test, EngineParameterFixture) {
  double new_engine_value {2.3984};
  engine_parameter.updateEngineValue(new_engine_value);
  double expected_python_value {7.127570618794628};
  BOOST_CHECK_EQUAL(expected_python_value, engine_parameter.getValue());
}

BOOST_AUTO_TEST_SUITE_END ()


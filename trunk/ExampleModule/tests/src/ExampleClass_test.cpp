/**
 * @file ExampleClass_test.cpp
 *
 * Created on: January 9, 2015
 *     Author: Pierre Dubath
 */

#include <boost/test/unit_test.hpp>
#include <boost/test/test_tools.hpp>
#include "ElementsKernel/Exception.h"
// class under test
#include "ExampleModule/ExampleClass.h"

namespace ecl = Euclid;
using namespace std;

// tolerance value to compare floating point numbers
double tolerance = 1e-12;

/*
 * Fixture to compare the test result against reference values
 */
struct ExampleClassFixture {

  std::string static_string { "This is a static field example" };
  int64_t source_id { 123456789 };
  double ra { 266.40506655 };
  double input_variable { 1.273645899 };
  double expected_result { 1.273645899 };

  ecl::ExampleClass example_class = ecl::ExampleClass::factoryMethod(source_id,
      ra);

  ExampleClassFixture() {
    //call constructor if needed
  }

  ~ExampleClassFixture() {
    // delete fixture object if needed
  }
};

BOOST_AUTO_TEST_SUITE (ExampleClass)

BOOST_AUTO_TEST_CASE(WithoutFixture) {
  BOOST_CHECK(true);
}

BOOST_FIXTURE_TEST_CASE(fundamentalTypeMethod_test, ExampleClassFixture) {
  BOOST_CHECK_CLOSE(expected_result,
      example_class.fundamentalTypeMethod(input_variable), tolerance);
}

BOOST_FIXTURE_TEST_CASE(Getter_test, ExampleClassFixture) {
  BOOST_CHECK_EQUAL(source_id, example_class.getSourceId());
}

BOOST_FIXTURE_TEST_CASE(exception_in_divideNumbers_test, ExampleClassFixture ) {
  //
  BOOST_CHECK_EXCEPTION(example_class.divideNumbers(1.0, 0.0), Elements::Exception,
      // below is a lambda function used as a predicate to check the exception error message
      [](const Elements::Exception& e){
            string exception_str = e.what();
            return exception_str.find("exception in ExampleClass::divideNumbers") != string::npos;
      }
  );
}

BOOST_AUTO_TEST_SUITE_END ()


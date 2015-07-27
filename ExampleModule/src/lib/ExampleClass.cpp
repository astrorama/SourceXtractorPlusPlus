/**
 * @file ExampleClass.cpp
 *
 * Created on: January 9, 2015
 *     Author: Pierre Dubath
 */

#include "ExampleModule/ExampleClass.h"
#include "ElementsKernel/Logging.h"
#include "ElementsKernel/Exception.h"

namespace Euclid {

using namespace std;

std::string ExampleClass::s_static_string = "This is a static field example";

double ExampleClass::fundamentalTypeMethod(const double input_variable) const {
  return input_variable;
}

/*
 * This is a silly example just to have a method throwing an exception
 */
double ExampleClass::divideNumbers(const double first,
    const double second) const {
  double tolerance = 1e-12;
  if (std::abs(second) < tolerance) {
    throw Elements::Exception() << "Dividing by " << second << " exception in ExampleClass::divideNumbers(...)";
  }
  return first / second;
}

void ExampleClass::passingUniquePointer(
    unique_ptr<vector<double>> vector_unique_ptr) const {
}

void ExampleClass::passingObjectInGeneral(
    const vector<double>& input_object) const {
}

ExampleClass ExampleClass::factoryMethod(const int64_t source_id,
    const double ra) {
  return ExampleClass { source_id, ra };
}

}// namespace Euclid

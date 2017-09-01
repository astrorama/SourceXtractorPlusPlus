/**
 * @file tests/src/PropertyId_test.cpp
 * @date 05/17/16
 * @author mschefer
 */

#include <boost/test/unit_test.hpp>

#include "SEFramework/Property/PropertyId.h"
#include "SEFramework/Property/Property.h"

using namespace SExtractor;

// Example property derived from PropertyImplementation
class ExamplePropertyA : public Property {
  // No content
};

// Example property derived from PropertyImplementation
class ExamplePropertyB : public Property {
  // No content
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (Property_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( example_test ) {
  PropertyId test_property_a(PropertyId::create<ExamplePropertyA>());
  PropertyId test_property_a2(PropertyId::create<ExamplePropertyA>());

  BOOST_CHECK(test_property_a == test_property_a2);
  BOOST_CHECK(std::hash<PropertyId>()(test_property_a) == std::hash<PropertyId>()(test_property_a2));

  PropertyId test_property_b(PropertyId::create<ExamplePropertyB>());
  BOOST_CHECK(!(test_property_a == test_property_b));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()



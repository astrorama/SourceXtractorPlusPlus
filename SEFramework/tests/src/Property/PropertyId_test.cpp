/**
 * @file tests/src/PropertyId_test.cpp
 * @date 05/17/16
 * @author mschefer
 */

#include <boost/test/unit_test.hpp>

#include "SEFramework/Property/Property.h"
#include "SEFramework/Property/PropertyId.h"

using namespace SExtractor;

// Example property derived from Property
class ExampleProperty : public Property {
  // No content
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (Property_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( example_test ) {

  PropertyId test_property(typeid(ExampleProperty));
  PropertyId test_property_with_empty_string(typeid(ExampleProperty), "");

  BOOST_CHECK(test_property == test_property_with_empty_string);
  BOOST_CHECK(std::hash<PropertyId>()(test_property) == std::hash<PropertyId>()(test_property_with_empty_string));

  PropertyId test_property_2(typeid(Property));
  BOOST_CHECK(!(test_property == test_property_2));

}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()



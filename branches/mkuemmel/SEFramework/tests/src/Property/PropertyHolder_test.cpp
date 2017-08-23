/**
 * @file tests/src/ObjectWithProperties_test.cpp
 * @date 05/03/16
 * @author mschefer
 */

#include "SEFramework/Property/PropertyHolder.h"

#include <memory.h>

#include <boost/test/unit_test.hpp>


#include "SEFramework/Property/Property.h"
#include "SEFramework/Source/SourceInterface.h"
#include "SEFramework/Property/PropertyNotFoundException.h"

using namespace SExtractor;

// Example property containing a string
class SimpleStringProperty : public Property {
public:
  std::string m_str;

  SimpleStringProperty(std::string str) {
    m_str = str;
  }
};

// Example property containing an int
class SimpleIntProperty : public Property {
public:
  int m_value;

  SimpleIntProperty(int value) : m_value(value) {}
};

// We want a test class that overrides the protected method isPropertySet() as a public method to be able to test it
class ObjectWithPropertiesTest : public PropertyHolder {
public:
  virtual bool isPropertySet(const PropertyId& property_id) const {
    return PropertyHolder::isPropertySet(property_id);
  }
};

struct ObjectWithPropertiesFixture {
  ObjectWithPropertiesTest object;

  const int magic_number = 42;
  std::string test_string { "this is a test string" };
  std::string test_string_2 { "this is another test string" };
  std::string test_property_name { "test" };
  std::string test_property_name_2 { "test2" };
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (ObjectWithProperties_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( getProperty_NotSet_test, ObjectWithPropertiesFixture ) {
  // check that accessing a property that was not set results in an exception
  BOOST_CHECK_THROW(object.getProperty(PropertyId::create<SimpleStringProperty>()), PropertyNotFoundException);
}

BOOST_FIXTURE_TEST_CASE( setGetProperty_DifferentTypes_test, ObjectWithPropertiesFixture ) {
  // Sets two properties of different types
  object.setProperty(std::unique_ptr<SimpleStringProperty>(new SimpleStringProperty(test_string)),
      PropertyId::create<SimpleStringProperty>());
  object.setProperty(std::unique_ptr<SimpleIntProperty>(new SimpleIntProperty(magic_number)),
      PropertyId::create<SimpleIntProperty>());

  // check that we can access them and that they contain what they should
  auto& string_property = dynamic_cast<const SimpleStringProperty&>(
      object.getProperty(PropertyId::create<SimpleStringProperty>()));
  BOOST_CHECK_EQUAL(string_property.m_str, test_string);

  auto& int_property = dynamic_cast<const SimpleIntProperty&>(
      object.getProperty(PropertyId::create<SimpleIntProperty>()));
  BOOST_CHECK_EQUAL(int_property.m_value, magic_number);
}


BOOST_FIXTURE_TEST_CASE( setGetProperty_SameType_test, ObjectWithPropertiesFixture ) {
  // Sets two properties of the same type with string parameters
  object.setProperty(std::unique_ptr<SimpleStringProperty>(new SimpleStringProperty(test_string)),
      PropertyId::create<SimpleStringProperty>() );
  object.setProperty(std::unique_ptr<SimpleStringProperty>(new SimpleStringProperty(test_string_2)),
      PropertyId::create<SimpleStringProperty>(1) );

  // check that we can access them and that they have their correct values
  auto string_property = dynamic_cast<const SimpleStringProperty&>(
      object.getProperty(PropertyId::create<SimpleStringProperty>()));
  BOOST_CHECK_EQUAL(string_property.m_str, test_string);

  auto string_property_2 = dynamic_cast<const SimpleStringProperty&>(
      object.getProperty(PropertyId::create<SimpleStringProperty>(1)));
  BOOST_CHECK_EQUAL(string_property_2.m_str, test_string_2);
}

BOOST_FIXTURE_TEST_CASE( isPropertySet_test, ObjectWithPropertiesFixture ) {
  // Test the result of isPropertySet() on a property that has not been set yet
  BOOST_CHECK(!object.isPropertySet(PropertyId::create<SimpleStringProperty>()));

  // Set that property
  object.setProperty(std::unique_ptr<SimpleStringProperty>(new SimpleStringProperty(test_string)),
      PropertyId::create<SimpleStringProperty>());

  // Check that it is now being reported as set
  BOOST_CHECK(object.isPropertySet(PropertyId::create<SimpleStringProperty>()));

  // Check that another property is still being reported as not set
  BOOST_CHECK(!object.isPropertySet(PropertyId::create<SimpleStringProperty>(1)));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()

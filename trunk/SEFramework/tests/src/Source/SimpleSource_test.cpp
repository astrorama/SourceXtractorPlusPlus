/*
 * SimpleSource_test.cpp
 *
 *  Created on: Aug 9, 2016
 *      Author: mschefer
 */

#include <boost/test/unit_test.hpp>

#include "SEFramework/Property/Property.h"
#include "SEFramework/Source/SimpleSource.h"

using namespace SExtractor;

// Example Property containing an int
class SimpleIntProperty : public Property {
public:
  int m_value;

  SimpleIntProperty(int value) : m_value(value) {}
};

struct SimpleSourceFixture {
  const int magic_number = 42;
  SimpleSource source;
};

BOOST_AUTO_TEST_SUITE (SimpleSource_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(SimpleSource_test, SimpleSourceFixture) {
  // Set a property
  source.setProperty<SimpleIntProperty>(magic_number);

  // Get our property back
  auto& property = source.getProperty<SimpleIntProperty>();

  // check that our property contains the correct value
  BOOST_CHECK_EQUAL(property.m_value, magic_number);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()

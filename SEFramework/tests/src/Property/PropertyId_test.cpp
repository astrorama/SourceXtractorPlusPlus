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
 * @file tests/src/PropertyId_test.cpp
 * @date 05/17/16
 * @author mschefer
 */

#include <boost/test/unit_test.hpp>

#include "SEFramework/Property/PropertyId.h"
#include "SEFramework/Property/Property.h"

using namespace SourceXtractor;

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



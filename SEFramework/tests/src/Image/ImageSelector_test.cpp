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
 * @file tests/src/Image/ImageSelector_test.cpp
 * @author embray
 */

#include <boost/test/unit_test.hpp>

#include "SEFramework/Image/ImageSelector.h"

using namespace SourceXtractor;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(ImageSelector_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(unspecified_test) {
  ImageSelector selector;

  BOOST_CHECK(!selector.isSpecified());
  BOOST_CHECK(!selector.isIndex());
  BOOST_CHECK(!selector.isName());
  BOOST_CHECK_EQUAL(selector.toString(), "");
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(index_test) {
  ImageSelector selector{2};

  BOOST_CHECK(selector.isSpecified());
  BOOST_CHECK(selector.isIndex());
  BOOST_CHECK(!selector.isName());
  BOOST_CHECK_EQUAL(selector.getIndex(), 2);
  BOOST_CHECK_EQUAL(selector.toString(), "2");
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(name_test) {
  ImageSelector selector{std::string{"SCI"}};

  BOOST_CHECK(selector.isSpecified());
  BOOST_CHECK(!selector.isIndex());
  BOOST_CHECK(selector.isName());
  BOOST_CHECK_EQUAL(selector.getName(), "SCI");
  BOOST_CHECK_EQUAL(selector.toString(), "SCI");
}

//-----------------------------------------------------------------------------

/// Text that is entirely an integer is an index
BOOST_AUTO_TEST_CASE(parse_index_test) {
  for (const auto& text : {"0", "1", "07", "-1"}) {
    auto selector = ImageSelector::parse(text);
    BOOST_CHECK_MESSAGE(selector.isIndex(), std::string{"not an index: "} + text);
  }

  BOOST_CHECK_EQUAL(ImageSelector::parse("07").getIndex(), 7);
  BOOST_CHECK_EQUAL(ImageSelector::parse("-1").getIndex(), -1);
}

//-----------------------------------------------------------------------------

/// Anything else is a name, including text that starts with digits (this was
/// previously buggy, see also parse_out_of_range_test)
BOOST_AUTO_TEST_CASE(parse_name_test) {
  for (const auto& text : {"SCI", "1x", "1.5", "", "/roman/data", "0x10"}) {
    auto selector = ImageSelector::parse(text);
    BOOST_CHECK_MESSAGE(selector.isName(), std::string{"not a name: "} + text);
    BOOST_CHECK_EQUAL(selector.getName(), text);
  }
}

//-----------------------------------------------------------------------------

/// An integer too large to parse is a name rather than an error
BOOST_AUTO_TEST_CASE(parse_out_of_range_test) {
  auto selector = ImageSelector::parse("99999999999999999999");

  BOOST_CHECK(selector.isName());
  BOOST_CHECK_EQUAL(selector.getName(), "99999999999999999999");
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(equality_test) {
  BOOST_CHECK(ImageSelector{1} == ImageSelector{1});
  BOOST_CHECK(ImageSelector{1} != ImageSelector{2});
  BOOST_CHECK(ImageSelector{std::string{"A"}} == ImageSelector{std::string{"A"}});
  BOOST_CHECK(ImageSelector{1} != ImageSelector{std::string{"1"}});
  BOOST_CHECK(ImageSelector{} != ImageSelector{0});
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()

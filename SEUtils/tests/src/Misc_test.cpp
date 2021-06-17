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


#include <boost/test/unit_test.hpp>

#include "SEUtils/Misc.h"

using namespace SourceXtractor;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (Misc_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( next_power_of_two_test ) {
  BOOST_CHECK_EQUAL(0, nextPowerOfTwo(0U));
  BOOST_CHECK_EQUAL(1, nextPowerOfTwo(1U));
  BOOST_CHECK_EQUAL(2, nextPowerOfTwo(2U));
  BOOST_CHECK_EQUAL(4, nextPowerOfTwo(3U));
  BOOST_CHECK_EQUAL(4, nextPowerOfTwo(4U));
  BOOST_CHECK_EQUAL(8, nextPowerOfTwo(5U));
  BOOST_CHECK_EQUAL(131072, nextPowerOfTwo(99999U));
  BOOST_CHECK_EQUAL(131072, nextPowerOfTwo(131072U));
  BOOST_CHECK_EQUAL(128, nextPowerOfTwo((unsigned char) 100));
  BOOST_CHECK_EQUAL(16384, nextPowerOfTwo((unsigned short) 10000));
  BOOST_CHECK_EQUAL(131072, nextPowerOfTwo(99999U));
  BOOST_CHECK_EQUAL(131072, nextPowerOfTwo(131072U));
  BOOST_CHECK_EQUAL(1UL << 45, nextPowerOfTwo(1UL << 45));
  BOOST_CHECK_EQUAL(1UL << 45, nextPowerOfTwo((1UL << 45) - 1));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()

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
 * @file tests/src/PixelCoordinate_test.cpp
 * @date 06/07/16
 * @author mschefer
 */

#include <boost/test/unit_test.hpp>

#include "SEUtils/PixelCoordinate.h"

using namespace SExtractor;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (PixelCoordinate_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( example_test ) {
  PixelCoordinate pc(1, 3);

  BOOST_CHECK_EQUAL(1, pc.m_x);
  BOOST_CHECK_EQUAL(3, pc.m_y);

  BOOST_CHECK(pc == PixelCoordinate(1, 3));
  BOOST_CHECK(pc != PixelCoordinate(3, 1));
  BOOST_CHECK(pc != PixelCoordinate(1, 6));
  BOOST_CHECK(pc != PixelCoordinate(3, 3));

  BOOST_CHECK(pc + PixelCoordinate(5, 9) == PixelCoordinate(6, 12));
  BOOST_CHECK(pc - PixelCoordinate(5, 2) == PixelCoordinate(-4, 1));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()

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
/*
 * OldSharp_test.cpp
 *
 *  Created on: July 02, 2019
 *      Author: Alejandro Alvarez Ayllon
 */

#include <boost/test/unit_test.hpp>
#include <cmath>
#include "ModelFitting/Models/OldSharp.h"

using namespace ModelFitting;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (OldSharp_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE (OldSharp_test) {
  // Radius of 4 pixels, start sampling at a radius of 1E-4, and increase in steps of *10 each time
  OldSharp sharp(4., 10., 1E-4);
  // Trigger the calculation of the radius
  // Note that r_max and Profile are not used by OldSharp
  sharp.updateRasterizationInfo(1., NAN, [](double) -> double { return 0.; });

  BOOST_CHECK(sharp.insideSharpRegion(0));
  BOOST_CHECK(sharp.insideSharpRegion(2));
  BOOST_CHECK(!sharp.insideSharpRegion(5));
  BOOST_CHECK(!sharp.insideSharpRegion(10));

  auto s = sharp.nextRadiusAndAngleNo(0.);
  BOOST_CHECK_EQUAL(s.first, 1E-4);
  BOOST_CHECK_EQUAL(s.second, 72);

  s = sharp.nextRadiusAndAngleNo(s.first);
  BOOST_CHECK_EQUAL(s.first, 1E-3);
  BOOST_CHECK_EQUAL(s.second, 72);

  s = sharp.nextRadiusAndAngleNo(s.first);
  BOOST_CHECK_EQUAL(s.first, 1E-2);
  BOOST_CHECK_EQUAL(s.second, 72);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()

//-----------------------------------------------------------------------------

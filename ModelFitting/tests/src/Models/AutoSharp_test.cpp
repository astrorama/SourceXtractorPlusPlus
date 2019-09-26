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
 * AutoSharp_test.cpp
 *
 *  Created on: July 02, 2019
 *      Author: Alejandro Alvarez Ayllon
 */

#include <boost/test/unit_test.hpp>
#include <cmath>
#include "ModelFitting/Models/AutoSharp.h"
#include "SEUtils/IsClose.h"

using namespace ModelFitting;
using namespace SExtractor;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (AutoSharp_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE (AutoSharp_test) {
  // Start sampling at a radius of 1E-4, and increase in steps of *10 each time.
  // Stop when the relative difference between the linear interpolation between two consecutive radius, and the
  // actual value is below 0.05.
  // Do five intermediate steps at the very least.
  AutoSharp sharp(10., 1e-4, 0.05, 5.);

  // Trigger the calculation of the radius
  // AutoSharp uses the function to evaluate the error at each step, so we need to use a real one!
  // We use a scale of 1 to 1
  sharp.updateRasterizationInfo(1., 20., [](double x) -> double { return x * x; });

  // Duh
  BOOST_CHECK(sharp.insideSharpRegion(0));

  // sqrt(2.0) = 1.4142135623730951
  // sqrt(3.0) = 1.7320508075688772
  // sqrt(2.5) = 1.5811388300841898
  // Linear interpolation for 2.5 (1.4142135623730951 + 1.7320508075688772) / 2 = 1.5731321849709863
  // Absolute error abs(1.5811388300841898 - 1.5731321849709863) = 0.008006645113203481
  // 0.008006645113203481 / 1.5811388300841898 = 0.005063846994875938 (which is above 0.05!)
  BOOST_CHECK(sharp.insideSharpRegion(2));
  BOOST_CHECK(sharp.insideSharpRegion(3));

  // sqrt(3.0) = 1.7320508075688772
  // sqrt(4.0) = 2
  // sqrt(3.5) = 1.8708286933869707
  // Linear interpolation for 3.5 (2 + 1.7320508075688772) / 2 = 1.8660254037844386
  // Absolute error abs(1.8708286933869707 - 1.8660254037844386) = 0.004803289602532068
  // 0.004803289602532068 / 1.8708286933869707 = 0.0025674662888755116  (which is below 0.05!)
  BOOST_CHECK(!sharp.insideSharpRegion(4));
  BOOST_CHECK(!sharp.insideSharpRegion(5));

  auto s = sharp.nextRadiusAndAngleNo(0.);
  BOOST_CHECK_EQUAL(s.first, 1E-4);
  BOOST_CHECK_EQUAL(s.second, 1);

  s = sharp.nextRadiusAndAngleNo(s.first);
  BOOST_CHECK_EQUAL(s.first, 1E-3);
  BOOST_CHECK_EQUAL(s.second, 1);

  s = sharp.nextRadiusAndAngleNo(s.first);
  BOOST_CHECK_EQUAL(s.first, 1E-2);
  BOOST_CHECK_EQUAL(s.second, 1);

  s = sharp.nextRadiusAndAngleNo(s.first);
  BOOST_CHECK_EQUAL(s.first, 1E-1);
  BOOST_CHECK_EQUAL(s.second, 1);

  s = sharp.nextRadiusAndAngleNo(s.first);
  BOOST_CHECK(isClose(s.first, 1E-1 + 1 / 5.)); // As 1 would go beyond the minimum scaling factor!
  BOOST_CHECK_EQUAL(s.second, 1);

  s = sharp.nextRadiusAndAngleNo(s.first);
  BOOST_CHECK(isClose(s.first, 1E-1 + 2 / 5.));
  BOOST_CHECK_EQUAL(s.second, 1);

  s = sharp.nextRadiusAndAngleNo(s.first);
  BOOST_CHECK(isClose(s.first, 1E-1 + 3 / 5.));
  BOOST_CHECK_EQUAL(s.second, 16.);

  s = sharp.nextRadiusAndAngleNo(s.first);
  BOOST_CHECK(isClose(s.first, 1E-1 + 4 / 5.));
  BOOST_CHECK_EQUAL(s.second, 22.);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()

//-----------------------------------------------------------------------------

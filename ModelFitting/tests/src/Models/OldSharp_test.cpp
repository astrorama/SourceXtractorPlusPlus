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

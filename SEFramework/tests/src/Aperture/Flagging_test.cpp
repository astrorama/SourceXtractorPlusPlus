/** Copyright © 2021 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
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
#include "SEFramework/Aperture/Flagging.h"

#include "Fixture.icpp"

using namespace SourceXtractor;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(Flagging_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(FlaggingNeighbor_test, FluxMeasurement_Fixture) {
  Flags flags = computeFlags(std::make_shared<CircularAperture>(1), 2, 2, detection_pixel_list,
                             detection_image, variance_map, detection_image, 1e2);

  BOOST_CHECK_EQUAL(flags, SourceXtractor::Flags::NEIGHBORS);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(FlaggingBoundary_test, FluxMeasurement_Fixture) {
  Flags flags = computeFlags(aperture, 1, 1, detection_pixel_list,
                             detection_image, variance_map, detection_image, 1e2);

  BOOST_CHECK_EQUAL(flags, SourceXtractor::Flags::BOUNDARY);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(FlaggingBoth_test, FluxMeasurement_Fixture) {
  Flags flags = computeFlags(aperture, 3, 2, detection_pixel_list,
                             detection_image, variance_map, detection_image, 1e2);

  BOOST_CHECK_EQUAL(flags, SourceXtractor::Flags::BOUNDARY | SourceXtractor::Flags::NEIGHBORS);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()

//-----------------------------------------------------------------------------

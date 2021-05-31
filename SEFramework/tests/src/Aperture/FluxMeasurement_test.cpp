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
#include "SEFramework/Aperture/CircularAperture.h"
#include "SEFramework/Aperture/FluxMeasurement.h"

#include "Fixture.icpp"

using namespace SourceXtractor;

BOOST_AUTO_TEST_SUITE(FluxMeasurement_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(CircularCentered_test, FluxMeasurement_Fixture) {
  auto measurement = measureFlux(aperture, 2, 2, detection_image, variance_map, 1e4, false);
  // The measurement will *not* flag if there are neighbors!
  // That's what computeFlags is for
  BOOST_CHECK_EQUAL(measurement.m_flags, SourceXtractor::Flags::NONE);
  BOOST_CHECK_CLOSE(measurement.m_flux, 133.48, 1e-2);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(Outside_test, FluxMeasurement_Fixture) {
  auto measurement = measureFlux(aperture, 20, 20, detection_image, variance_map, 1e4, false);
  BOOST_CHECK_EQUAL(measurement.m_flags, SourceXtractor::Flags::OUTSIDE);
  BOOST_CHECK_EQUAL(measurement.m_flux, 0.);
  BOOST_CHECK_EQUAL(measurement.m_total_area, 0.);
  BOOST_CHECK_EQUAL(measurement.m_variance, 0.);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(VarThresholdAll_test, FluxMeasurement_Fixture) {
  auto measurement = measureFlux(aperture, 2, 2, detection_image, variance_map, 1e-5, false);
  BOOST_CHECK_EQUAL(measurement.m_flags, SourceXtractor::Flags::BIASED);
  BOOST_CHECK_EQUAL(measurement.m_flux, 0.);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(VarThresholdSome_test, FluxMeasurement_Fixture) {
  auto measurement = measureFlux(aperture, 2, 2, detection_image, variance_map, 0.4, false);
  BOOST_CHECK_EQUAL(measurement.m_flags, SourceXtractor::Flags::BIASED);
  BOOST_CHECK_CLOSE(measurement.m_flux, 105.48, 1e-2);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(Boundary_test, FluxMeasurement_Fixture) {
  auto measurement = measureFlux(aperture, 3, 2, detection_image, variance_map, 1e5, false);
  BOOST_CHECK_EQUAL(measurement.m_flags, SourceXtractor::Flags::BOUNDARY);
  BOOST_CHECK_CLOSE(measurement.m_flux, 139.96, 1e-2);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()

//-----------------------------------------------------------------------------

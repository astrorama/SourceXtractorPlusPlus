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
#include "SEFramework/Source/SimpleSource.h"
#include "SEImplementation/Plugin/FluxRadius/FluxRadius.h"
#include "SEImplementation/Plugin/FluxRadius/FluxRadiusTask.h"
#include "SEImplementation/Plugin/GrowthCurve/GrowthCurve.h"

using namespace SourceXtractor;

struct FluxRadiusFixture {
  SimpleSource source;

  FluxRadiusFixture() {
    source.setProperty<GrowthCurve>(std::vector<double>{0.1, 0.2, 0.5, 0.6, 0.9, 1.0, 1.0}, 7);
  };
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (FluxRadius_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE (One_test, FluxRadiusFixture) {
  FluxRadiusTask task{{0}, {0.5}};

  task.computeProperties(source);
  auto& flux_radius_prop = source.getProperty<FluxRadius>();
  auto& flux_radius = flux_radius_prop.getFluxRadius();

  BOOST_CHECK_EQUAL(flux_radius.shape()[0], 1);
  BOOST_CHECK_EQUAL(flux_radius.shape()[1], 1);
  BOOST_CHECK_EQUAL(flux_radius.at(0, 0), 3.); // Note that the first element is 1!
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE (Interpolate_test, FluxRadiusFixture) {
  FluxRadiusTask task{{0}, {0.55}};

  task.computeProperties(source);
  auto& flux_radius_prop = source.getProperty<FluxRadius>();
  auto& flux_radius = flux_radius_prop.getFluxRadius();

  BOOST_CHECK_EQUAL(flux_radius.shape()[0], 1);
  BOOST_CHECK_EQUAL(flux_radius.shape()[1], 1);
  BOOST_CHECK_CLOSE(flux_radius.at(0, 0), 3.5, 1e-5); // Note that the first element is 1!
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE (N_test, FluxRadiusFixture) {
  FluxRadiusTask task{{0}, {0.2, 0.5, 0.6}};

  task.computeProperties(source);
  auto& flux_radius_prop = source.getProperty<FluxRadius>();
  auto& flux_radius = flux_radius_prop.getFluxRadius();

  BOOST_CHECK_EQUAL(flux_radius.shape()[0], 1);
  BOOST_CHECK_EQUAL(flux_radius.shape()[1], 3);
  BOOST_CHECK_EQUAL(flux_radius.at(0, 0), 2.);
  BOOST_CHECK_EQUAL(flux_radius.at(0, 1), 3.);
  BOOST_CHECK_EQUAL(flux_radius.at(0, 2), 4.);
}

//-----------------------------------------------------------------------------

// Asking for the flux fraction 0 (or negative) should give 0
// Note that the configuration should catch these, but the code should be robust
BOOST_FIXTURE_TEST_CASE (ZeroFrac_test, FluxRadiusFixture) {
  FluxRadiusTask task{{0}, {0., -0.5}};

  task.computeProperties(source);
  auto& flux_radius_prop = source.getProperty<FluxRadius>();
  auto& flux_radius = flux_radius_prop.getFluxRadius();

  BOOST_CHECK_EQUAL(flux_radius.shape()[0], 1);
  BOOST_CHECK_EQUAL(flux_radius.shape()[1], 2);
  BOOST_CHECK_EQUAL(flux_radius.at(0, 0), 0.);
  BOOST_CHECK_EQUAL(flux_radius.at(0, 1), 0.);
}

//-----------------------------------------------------------------------------

// Asking for the flux fraction > 1 should give the last bin, and 1 the first where the CDF = 1
// Note that the configuration should catch these, but the code should be robust
BOOST_FIXTURE_TEST_CASE (Ge1_test, FluxRadiusFixture) {
  FluxRadiusTask task{{0}, {1., 5.}};

  task.computeProperties(source);
  auto& flux_radius_prop = source.getProperty<FluxRadius>();
  auto& flux_radius = flux_radius_prop.getFluxRadius();

  BOOST_CHECK_EQUAL(flux_radius.shape()[0], 1);
  BOOST_CHECK_EQUAL(flux_radius.shape()[1], 2);
  BOOST_CHECK_EQUAL(flux_radius.at(0, 0), 6.);
  BOOST_CHECK_EQUAL(flux_radius.at(0, 1), 7.);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()

//-----------------------------------------------------------------------------

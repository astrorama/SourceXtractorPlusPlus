/** Copyright © 2019-2026 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
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
#include <cmath>
#include <limits>

#include "SEFramework/Source/SimpleSource.h"
#include "SEImplementation/Plugin/SpreadModelAlt/CombinedSpreadModelAlt.h"
#include "SEImplementation/Plugin/SpreadModelAlt/CombinedSpreadModelAltTask.h"
#include "SEImplementation/Plugin/SpreadModelAlt/SpreadModelAlt.h"
#include "SEImplementation/Plugin/SpreadModelAlt/SpreadModelAltTaskFactory.h"
#include "SEUtils/IsNan.h"

using namespace SourceXtractor;
BOOST_AUTO_TEST_SUITE(CombinedSpreadModelAlt_test)

BOOST_AUTO_TEST_CASE(equal_weights_and_frame_isolation) {
  SimpleSource source;
  source.setIndexedProperty<SpreadModelAlt>(3, 1, 2);
  source.setIndexedProperty<SpreadModelAlt>(8, 3, 2);
  CombinedSpreadModelAltTask({3, 8}).computeProperties(source);
  const auto& result = source.getProperty<CombinedSpreadModelAlt>();
  BOOST_CHECK_EQUAL(result.getSpreadModelAlt(), 2);
  BOOST_CHECK_CLOSE(result.getSpreadModelAltError(), std::sqrt(2.0), 1e-10);
  BOOST_CHECK_EQUAL(result.getNFrames(), 2);
  BOOST_CHECK_EQUAL(result.getReducedChiSquared(), 0.5);
  BOOST_CHECK_EQUAL(source.getProperty<SpreadModelAlt>(3).getSpreadModelAlt(), 1);
  BOOST_CHECK_EQUAL(source.getProperty<SpreadModelAlt>(8).getSpreadModelAlt(), 3);
}

BOOST_AUTO_TEST_CASE(unequal_weights_and_order) {
  SimpleSource forward, reverse;
  for (auto* source : {&forward, &reverse}) {
    source->setIndexedProperty<SpreadModelAlt>(2, 1, 1);
    source->setIndexedProperty<SpreadModelAlt>(5, 3, 2);
  }
  CombinedSpreadModelAltTask({2, 5}).computeProperties(forward);
  CombinedSpreadModelAltTask({5, 2}).computeProperties(reverse);
  const auto& a = forward.getProperty<CombinedSpreadModelAlt>();
  const auto& b = reverse.getProperty<CombinedSpreadModelAlt>();
  BOOST_CHECK_CLOSE(a.getSpreadModelAlt(), 1.4, 1e-10);
  BOOST_CHECK_CLOSE(a.getSpreadModelAltError(), std::sqrt(0.8), 1e-10);
  BOOST_CHECK_CLOSE(a.getReducedChiSquared(), 0.8, 1e-10);
  BOOST_CHECK_EQUAL(a.getSpreadModelAlt(), b.getSpreadModelAlt());
  BOOST_CHECK_EQUAL(a.getSpreadModelAltError(), b.getSpreadModelAltError());
}

BOOST_AUTO_TEST_CASE(invalid_and_single_frame) {
  SimpleSource source;
  const auto nan = std::numeric_limits<SeFloat>::quiet_NaN();
  const auto inf = std::numeric_limits<SeFloat>::infinity();
  source.setIndexedProperty<SpreadModelAlt>(0, nan, 1);
  source.setIndexedProperty<SpreadModelAlt>(1, inf, 1);
  source.setIndexedProperty<SpreadModelAlt>(2, 1, nan);
  source.setIndexedProperty<SpreadModelAlt>(3, 1, inf);
  source.setIndexedProperty<SpreadModelAlt>(4, 1, 0);
  source.setIndexedProperty<SpreadModelAlt>(5, 1, -1);
  source.setIndexedProperty<SpreadModelAlt>(6, -0.25, 0.125);
  CombinedSpreadModelAltTask({0, 1, 2, 3, 4, 5, 6}).computeProperties(source);
  const auto& result = source.getProperty<CombinedSpreadModelAlt>();
  BOOST_CHECK_EQUAL(result.getSpreadModelAlt(), -0.25);
  BOOST_CHECK_EQUAL(result.getSpreadModelAltError(), 0.125);
  BOOST_CHECK_EQUAL(result.getNFrames(), 1);
  BOOST_CHECK(fastmath_isnan(result.getReducedChiSquared()));
  SimpleSource invalid;
  invalid.setIndexedProperty<SpreadModelAlt>(7, 1, 0);
  CombinedSpreadModelAltTask({7}).computeProperties(invalid);
  const auto& empty = invalid.getProperty<CombinedSpreadModelAlt>();
  BOOST_CHECK_EQUAL(empty.getNFrames(), 0);
  BOOST_CHECK(fastmath_isnan(empty.getSpreadModelAlt()));
  BOOST_CHECK(fastmath_isnan(empty.getSpreadModelAltError()));
  BOOST_CHECK(fastmath_isnan(empty.getReducedChiSquared()));
}

BOOST_AUTO_TEST_CASE(empty_configuration_and_factory) {
  SimpleSource source;
  CombinedSpreadModelAltTask({}).computeProperties(source);
  BOOST_CHECK_EQUAL(source.getProperty<CombinedSpreadModelAlt>().getNFrames(), 0);
  SpreadModelAltTaskFactory factory;
  BOOST_CHECK(factory.createTask(PropertyId::create<CombinedSpreadModelAlt>()) != nullptr);
  BOOST_CHECK(factory.createTask(PropertyId::create<CombinedSpreadModelAlt>(1)) == nullptr);
}

BOOST_AUTO_TEST_CASE(extreme_uncertainty_ratio) {
  SimpleSource source;
  source.setIndexedProperty<SpreadModelAlt>(0, 2, 1e-30f);
  source.setIndexedProperty<SpreadModelAlt>(1, 100, 1e30f);
  CombinedSpreadModelAltTask({0, 1}).computeProperties(source);
  const auto& result = source.getProperty<CombinedSpreadModelAlt>();
  BOOST_CHECK_EQUAL(result.getSpreadModelAlt(), 2);
  BOOST_CHECK_EQUAL(result.getSpreadModelAltError(), double(SeFloat(1e-30f)));
  BOOST_CHECK_EQUAL(result.getNFrames(), 2);
  BOOST_CHECK(!fastmath_isinf(result.getReducedChiSquared()));
  BOOST_CHECK(!fastmath_isnan(result.getReducedChiSquared()));
}
BOOST_AUTO_TEST_SUITE_END()

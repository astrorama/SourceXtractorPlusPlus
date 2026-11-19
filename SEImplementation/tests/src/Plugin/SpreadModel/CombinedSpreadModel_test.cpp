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
#include "SEImplementation/Plugin/SpreadModel/CombinedSpreadModel.h"
#include "SEImplementation/Plugin/SpreadModel/CombinedSpreadModelTask.h"
#include "SEImplementation/Plugin/SpreadModel/SpreadModel.h"
#include "SEImplementation/Plugin/SpreadModel/SpreadModelTaskFactory.h"
#include "SEUtils/IsNan.h"

using namespace SourceXtractor;
BOOST_AUTO_TEST_SUITE(CombinedSpreadModel_test)

BOOST_AUTO_TEST_CASE(equal_weights_and_frame_isolation) {
  SimpleSource source;
  source.setIndexedProperty<SpreadModel>(3, 1, 2);
  source.setIndexedProperty<SpreadModel>(8, 3, 2);
  CombinedSpreadModelTask({3, 8}).computeProperties(source);
  const auto& result = source.getProperty<CombinedSpreadModel>();
  BOOST_CHECK_EQUAL(result.getSpreadModel(), 2);
  BOOST_CHECK_CLOSE(result.getSpreadModelError(), std::sqrt(2.0), 1e-10);
  BOOST_CHECK_EQUAL(result.getNFrames(), 2);
  BOOST_CHECK_EQUAL(result.getReducedChiSquared(), 0.5);
  BOOST_CHECK_EQUAL(source.getProperty<SpreadModel>(3).getSpreadModel(), 1);
  BOOST_CHECK_EQUAL(source.getProperty<SpreadModel>(8).getSpreadModel(), 3);
}

BOOST_AUTO_TEST_CASE(unequal_weights_and_order) {
  SimpleSource forward, reverse;
  for (auto* source : {&forward, &reverse}) {
    source->setIndexedProperty<SpreadModel>(2, 1, 1);
    source->setIndexedProperty<SpreadModel>(5, 3, 2);
  }
  CombinedSpreadModelTask({2, 5}).computeProperties(forward);
  CombinedSpreadModelTask({5, 2}).computeProperties(reverse);
  const auto& a = forward.getProperty<CombinedSpreadModel>();
  const auto& b = reverse.getProperty<CombinedSpreadModel>();
  BOOST_CHECK_CLOSE(a.getSpreadModel(), 1.4, 1e-10);
  BOOST_CHECK_CLOSE(a.getSpreadModelError(), std::sqrt(0.8), 1e-10);
  BOOST_CHECK_CLOSE(a.getReducedChiSquared(), 0.8, 1e-10);
  BOOST_CHECK_EQUAL(a.getSpreadModel(), b.getSpreadModel());
  BOOST_CHECK_EQUAL(a.getSpreadModelError(), b.getSpreadModelError());
}

BOOST_AUTO_TEST_CASE(invalid_and_single_frame) {
  SimpleSource source;
  const auto nan = std::numeric_limits<SeFloat>::quiet_NaN();
  const auto inf = std::numeric_limits<SeFloat>::infinity();
  source.setIndexedProperty<SpreadModel>(0, nan, 1);
  source.setIndexedProperty<SpreadModel>(1, inf, 1);
  source.setIndexedProperty<SpreadModel>(2, 1, nan);
  source.setIndexedProperty<SpreadModel>(3, 1, inf);
  source.setIndexedProperty<SpreadModel>(4, 1, 0);
  source.setIndexedProperty<SpreadModel>(5, 1, -1);
  source.setIndexedProperty<SpreadModel>(6, -0.25, 0.125);
  CombinedSpreadModelTask({0, 1, 2, 3, 4, 5, 6}).computeProperties(source);
  const auto& result = source.getProperty<CombinedSpreadModel>();
  BOOST_CHECK_EQUAL(result.getSpreadModel(), -0.25);
  BOOST_CHECK_EQUAL(result.getSpreadModelError(), 0.125);
  BOOST_CHECK_EQUAL(result.getNFrames(), 1);
  BOOST_CHECK(fastmath_isnan(result.getReducedChiSquared()));
  SimpleSource invalid;
  invalid.setIndexedProperty<SpreadModel>(7, 1, 0);
  CombinedSpreadModelTask({7}).computeProperties(invalid);
  const auto& empty = invalid.getProperty<CombinedSpreadModel>();
  BOOST_CHECK_EQUAL(empty.getNFrames(), 0);
  BOOST_CHECK(fastmath_isnan(empty.getSpreadModel()));
  BOOST_CHECK(fastmath_isnan(empty.getSpreadModelError()));
  BOOST_CHECK(fastmath_isnan(empty.getReducedChiSquared()));
}

BOOST_AUTO_TEST_CASE(empty_configuration_and_factory) {
  SimpleSource source;
  CombinedSpreadModelTask({}).computeProperties(source);
  BOOST_CHECK_EQUAL(source.getProperty<CombinedSpreadModel>().getNFrames(), 0);
  SpreadModelTaskFactory factory;
  BOOST_CHECK(factory.createTask(PropertyId::create<CombinedSpreadModel>()) != nullptr);
  BOOST_CHECK(factory.createTask(PropertyId::create<CombinedSpreadModel>(1)) == nullptr);
}

BOOST_AUTO_TEST_CASE(extreme_uncertainty_ratio) {
  SimpleSource source;
  source.setIndexedProperty<SpreadModel>(0, 2, 1e-30f);
  source.setIndexedProperty<SpreadModel>(1, 100, 1e30f);
  CombinedSpreadModelTask({0, 1}).computeProperties(source);
  const auto& result = source.getProperty<CombinedSpreadModel>();
  BOOST_CHECK_EQUAL(result.getSpreadModel(), 2);
  BOOST_CHECK_EQUAL(result.getSpreadModelError(), double(SeFloat(1e-30f)));
  BOOST_CHECK_EQUAL(result.getNFrames(), 2);
  BOOST_CHECK(!fastmath_isinf(result.getReducedChiSquared()));
  BOOST_CHECK(!fastmath_isnan(result.getReducedChiSquared()));
}
BOOST_AUTO_TEST_SUITE_END()

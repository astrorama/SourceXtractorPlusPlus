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
 * @file tests/src/OverlappingBoundariesCriteria_test.cpp
 * @date 06/07/16
 * @author mschefer
 */

#include <boost/test/unit_test.hpp>

#include "SEFramework/Source/SimpleSource.h"

#include "SEImplementation/Property/PixelCoordinateList.h"
#include "SEImplementation/Plugin/PixelBoundaries/PixelBoundaries.h"

#include "SEImplementation/Grouping/OverlappingBoundariesCriteria.h"

using namespace SourceXtractor;

struct OverlappingBoundariesCriteriaFixture {
  SimpleSource source_a, source_b, source_c;
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (OverlappingBoundariesCriteria_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(OverlappingBoundariesCriteriaTest, OverlappingBoundariesCriteriaFixture) {
  source_a.setProperty<PixelCoordinateList>(std::vector<PixelCoordinate>{{1,3}, {2,4}});
  source_a.setProperty<PixelBoundaries>(1, 3, 2, 4);

  source_b.setProperty<PixelCoordinateList>(std::vector<PixelCoordinate>{{1,4}, {1,5}});
  source_b.setProperty<PixelBoundaries>(1, 4, 1, 5);

  source_c.setProperty<PixelCoordinateList>(std::vector<PixelCoordinate>{{1,5}, {1,6}});
  source_c.setProperty<PixelBoundaries>(1, 5, 1, 6);

  BOOST_CHECK(OverlappingBoundariesCriteria().shouldGroup(source_a, source_a));
  BOOST_CHECK(OverlappingBoundariesCriteria().shouldGroup(source_a, source_b));
  BOOST_CHECK(!OverlappingBoundariesCriteria().shouldGroup(source_a, source_c));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()



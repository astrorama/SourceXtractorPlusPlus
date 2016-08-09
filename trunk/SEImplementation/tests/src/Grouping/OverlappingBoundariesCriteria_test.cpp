/**
 * @file tests/src/OverlappingBoundariesCriteria_test.cpp
 * @date 06/07/16
 * @author mschefer
 */

#include <boost/test/unit_test.hpp>

#include "SEFramework/Source/SimpleSource.h"

#include "SEImplementation/Property/PixelCoordinateList.h"
#include "SEImplementation/Property/PixelBoundaries.h"

#include "SEImplementation/Grouping/OverlappingBoundariesCriteria.h"

using namespace SExtractor;

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



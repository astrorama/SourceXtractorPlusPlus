/**
 * @file tests/src/PixelBoundaries_test.cpp
 * @date 05/30/16
 * @author mschefer
 */

#include <boost/test/unit_test.hpp>

#include "SEFramework/Source/SimpleSource.h"

#include "SEImplementation/Property/PixelCoordinateList.h"
#include "SEImplementation/Property/PixelBoundaries.h"
#include "SEImplementation/Task/PixelBoundariesTask.h"

using namespace SExtractor;

struct PixelBoundariesFixture {
  SimpleSource source;
  PixelBoundariesTask pixel_centroid_task;
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (PixelBoundaries_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( boundaries_test, PixelBoundariesFixture ) {
  source.setProperty<PixelCoordinateList>(std::vector<PixelCoordinate>{{1,3}, {8,-4}});

  pixel_centroid_task.computeProperties(source);

  auto boundaries = source.getProperty<PixelBoundaries>();
  BOOST_CHECK(boundaries.getMin() == PixelCoordinate(1, -4));
  BOOST_CHECK(boundaries.getMax() == PixelCoordinate(8, 3));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()



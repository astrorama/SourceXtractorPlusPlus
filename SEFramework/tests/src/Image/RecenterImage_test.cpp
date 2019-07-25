/**
 * @file tests/src/RecenterImage_test.cpp
 * @date 11/09/18
 * @author Alejandro Alvarez Ayllon
 */

#include <boost/test/unit_test.hpp>
#include "SEFramework/Image/MirrorImage.h"
#include "SEFramework/Image/PaddedImage.h"
#include "SEFramework/Image/RecenterImage.h"
#include "SEFramework/Image/VectorImage.h"

#include "SEUtils/TestUtils.h"

using namespace SExtractor;

struct RecenterImage_Fixture {
  std::shared_ptr<VectorImage<SeFloat>> img;

  RecenterImage_Fixture() : img{VectorImage<SeFloat>::create(
    3, 3,
    std::vector<SeFloat>{
      1, 2, 3,
      4, 5, 6,
      7, 8, 9
    })} {
  }
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (RecenterImage_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE (RecenterImage_test, RecenterImage_Fixture) {
  auto recentered = RecenterImage<SeFloat>::create(img, PixelCoordinate{1, 1});

  auto expected = VectorImage<SeFloat>::create(3, 3, std::vector<SeFloat>{
    5, 6, 4,
    8, 9, 7,
    2, 3, 1
  });

  BOOST_CHECK(compareImages(expected, recentered));
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE (RecenterChained_test, RecenterImage_Fixture) {
  auto mirrored = MirrorImage<SeFloat>::create(img);
  auto padded = PaddedImage<SeFloat>::create(mirrored, 5, 5);
  auto recenter = RecenterImage<SeFloat>::create(padded, PixelCoordinate{2, 2});

  auto expected = VectorImage<SeFloat>::create(5, 5, std::vector<SeFloat>{
    5, 4, 0, 0, 6,
    2, 1, 0, 0, 3,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    8, 7, 0, 0, 9,
  });

  BOOST_CHECK(compareImages(expected, recenter));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()

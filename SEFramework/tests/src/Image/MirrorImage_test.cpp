/**
 * @file tests/src/MirrorImage_test.cpp
 * @date 11/09/18
 * @author Alejandro Alvarez Ayllon
 */

#include <boost/test/unit_test.hpp>
#include "SEFramework/Image/MirrorImage.h"
#include "SEFramework/Image/VectorImage.h"

#include "SEUtils/TestUtils.h"

using namespace SExtractor;

struct MirrorImage_Fixture {
  std::shared_ptr<VectorImage<SeFloat>> img;

  MirrorImage_Fixture() : img{VectorImage<SeFloat>::create(
    3, 3,
    std::vector<SeFloat>{
      1, 2, 3,
      4, 5, 6,
      7, 8, 9
    })} {
  }
};


//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (MirrorImage_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE (MirrorImage_test, MirrorImage_Fixture) {
  auto recentered = MirrorImage<SeFloat>::create(img);

  auto expected = VectorImage<SeFloat>::create(3, 3, std::vector<SeFloat>{
    9, 8, 7,
    6, 5, 4,
    3, 2, 1
  });

  BOOST_CHECK(compareImages(expected, recentered));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()

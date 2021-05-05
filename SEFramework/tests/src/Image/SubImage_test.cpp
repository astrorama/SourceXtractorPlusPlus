/*
 * SubImage_test.cpp
 *
 *  Created on: May 4, 2021
 *      Author: mschefer
 */


#include <boost/test/unit_test.hpp>
#include "SEFramework/Image/SubImage.h"
#include "SEFramework/Image/VectorImage.h"

#include "SEUtils/TestUtils.h"



using namespace SourceXtractor;

struct SubImage_Fixture {
  std::shared_ptr<VectorImage<SeFloat>> img;

  SubImage_Fixture() : img { VectorImage<SeFloat>::create(
    3, 3,
    std::vector<SeFloat>{
      1, 2, 3,
      4, 5, 6,
      7, 8, 9
    })} {
  }
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (SubImage_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE (RecenterImage_test, SubImage_Fixture) {
  auto sub_image = SubImage<SeFloat>::create(img, PixelCoordinate{1, 1}, 2, 2);

  auto expected_image = VectorImage<SeFloat>::create(2, 2, std::vector<SeFloat>{
    5, 6,
    8, 9
  });

  BOOST_CHECK(compareImages(sub_image, expected_image));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()

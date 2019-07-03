/**
 * @file tests/src/TransformedImage_test.cpp
 * @date 27/03/19
 * @author Alejandro Alvarez Ayllon
 */

#include <boost/test/unit_test.hpp>
#include "SEFramework/Image/FunctionalImage.h"
#include "SEFramework/Image/VectorImage.h"

#include "SEUtils/TestUtils.h"

using namespace SExtractor;

BOOST_AUTO_TEST_SUITE (FunctionalImage_test)

BOOST_AUTO_TEST_CASE (image_to_mask) {
  auto image = VectorImage<SeFloat>::create(
    5, 5,
    std::vector<SeFloat>{
      1, 2, 3, 4, 5,
      6, 7, 8, 9, 10,
      11, 12, 13, 14, 15,
      16, 17, 18, 19, 20,
      21, 22, 23, 24, 25
    });

  auto transformed = FunctionalImage<int>::create(
    5, 5, [image](int x, int y) -> int {
      return image->getValue(x, y) >= 12 && image->getValue(x, y) <= 14;
    }
  );

  auto expected = VectorImage<int>::create(
    5, 5,
    std::vector<int>{
      0, 0, 0, 0, 0,
      0, 0, 0, 0, 0,
      0, 1, 1, 1, 0,
      0, 0, 0, 0, 0,
      0, 0, 0, 0, 0,
    });

  BOOST_CHECK(compareImages(expected, transformed));
}

BOOST_AUTO_TEST_SUITE_END ()

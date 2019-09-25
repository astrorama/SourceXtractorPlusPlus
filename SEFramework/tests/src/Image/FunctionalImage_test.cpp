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

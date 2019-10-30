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
 * @file tests/src/MirrorImage_test.cpp
 * @date 11/09/18
 * @author Alejandro Alvarez Ayllon
 */

#include <boost/test/unit_test.hpp>
#include "SEFramework/Image/MirrorImage.h"
#include "SEFramework/Image/VectorImage.h"

#include "SEUtils/TestUtils.h"

using namespace SourceXtractor;

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

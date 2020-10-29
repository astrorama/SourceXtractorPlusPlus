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

#include <boost/test/unit_test.hpp>

#include "SEImplementation/Background/SE/ReplaceUndefImage.h"
#include "SEFramework/Image/VectorImage.h"
#include "SEUtils/TestUtils.h"

using namespace SourceXtractor;

auto INV = std::numeric_limits<SeFloat>::min();

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(ReplaceUndefImage_test)

//-----------------------------------------------------------------------------
// There is one single missing pixel, with 4 close values: top, bottom, left and right
// (diagonal elements are further away!)
//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(replaceInvOne) {
  std::shared_ptr<VectorImage<SeFloat>> one_missing = VectorImage<SeFloat>::create(
    5, 5, std::vector<SeFloat>{
      1, 2, 3, 4, 5,
      1, 2, 2, 4, 5,
      1, 2, INV, 4, 5,
      1, 2, 3, 4, 5,
      1, 2, 3, 4, 5,
    }
  );

  std::shared_ptr<VectorImage<SeFloat>> expected = VectorImage<SeFloat>::create(
    5, 5, std::vector<SeFloat>{
      1, 2, 3, 4, 5,
      1, 2, 2, 4, 5,
      1, 2, 2.75, 4, 5,
      1, 2, 3, 4, 5,
      1, 2, 3, 4, 5,
    }
  );

  auto replaced = ReplaceUndefImage<SeFloat>::create(one_missing, INV);
  BOOST_CHECK(compareImages(expected, replaced));
}

//-----------------------------------------------------------------------------
// There are two close missing pixels. They should not influence each other
//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(replaceInvTwo) {
  std::shared_ptr<VectorImage<SeFloat>> two_missing = VectorImage<SeFloat>::create(
    5, 5, std::vector<SeFloat>{
      1, 2, 3, 4, 5,
      1, 2, 2, 4, 5,
      1, 2, INV, INV, 5,
      1, 2, 3, 4, 5,
      1, 2, 3, 4, 5,
    }
  );

  std::shared_ptr<VectorImage<SeFloat>> expected = VectorImage<SeFloat>::create(
    5, 5, std::vector<SeFloat>{
      1, 2, 3, 4, 5,
      1, 2, 2, 4, 5,
      1, 2, 2.3333, 4.33333, 5,
      1, 2, 3, 4, 5,
      1, 2, 3, 4, 5,
    }
  );

  auto replaced = ReplaceUndefImage<SeFloat>::create(two_missing, INV);
  BOOST_CHECK(compareImages(expected, replaced, 1e-3));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()

//-----------------------------------------------------------------------------

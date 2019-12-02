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
 * @file tests/src/PixelBoundaries_test.cpp
 * @date 05/30/16
 * @author mschefer
 */

#include <boost/test/unit_test.hpp>

#include "SEFramework/Source/SimpleSource.h"

#include "SEImplementation/Property/PixelCoordinateList.h"
#include "SEImplementation/Plugin/PixelBoundaries/PixelBoundaries.h"
#include "SEImplementation/Plugin/PixelBoundaries/PixelBoundariesTask.h"

using namespace SourceXtractor;

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



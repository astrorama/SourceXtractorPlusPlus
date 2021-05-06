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
/*
 *  NeighbourInfo_test.cpp
 *
 *  Created on: Oct 02, 2018
 *      Author: Alejandro Alvarez Ayllon
 */

#include <boost/test/unit_test.hpp>

#include "SEFramework/Image/ConstantImage.h"
#include "SEFramework/Aperture/NeighbourInfo.h"

#include "Fixture.icpp"

using namespace SourceXtractor;

BOOST_AUTO_TEST_SUITE(NeighbourInfo_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(OneToOne_test, FluxMeasurement_Fixture) {
  NeighbourInfo neighbour_info{min_pixel, max_pixel, detection_pixel_list, detection_image};

  auto expected = VectorImage<int>::create(3, 3, std::vector<int>{
    0, 0, 1,
    0, 0, 0,
    0, 0, 0,
  });

  for (int y = 0; y < expected->getHeight(); ++y) {
    for (int x = 0; x < expected->getWidth(); ++x) {
      BOOST_CHECK_EQUAL(expected->getValue(x, y), neighbour_info.isNeighbourObjectPixel(min_pixel.m_x + x, min_pixel.m_y + y));
    }
  }
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()

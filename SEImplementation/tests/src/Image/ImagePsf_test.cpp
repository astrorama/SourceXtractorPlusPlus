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
 * ImagePsf_test.cpp
 *
 *  Created on: Jul 18, 2018
 *      Author: Alejandro Álvarez
 */

#include <boost/test/unit_test.hpp>

#include "SEImplementation/Image/ImagePsf.h"

using namespace SourceXtractor;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (ImagePsf_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE (convolution) {
  // Note: This test assumes a behaviour like BORDER_REFLECT_101 is used
  // gfedcb|abcdefgh|gfedcba

  ImagePsf kernel(1, VectorImage<SeFloat>::create(3, 3, std::vector<SeFloat>{
    0.0, 0.5, 0.0,
    0.5, 1.0, 0.5,
    0.0, 0.5, 0.0
  }));
  auto image = VectorImage<SeFloat>::create(5, 5, std::vector<SeFloat>{
    0.5, 0.6, 0.5, 0.4, 0.2,
    0.7, 0.6, 0.6, 0.1, 0.4,
    0.3, 0.1, 0.3, 0.2, 1.0,
    0.5, 0.1, 0.8, 0.5, 1.0,
    0.2, 0.3, 0.4, 0.4, 0.6
  });

  kernel.convolve(image);

  auto expected = VectorImage<SeFloat>::create(5, 5, std::vector<SeFloat>{
    1.80, 1.70, 1.60, 0.85, 1.00,
    1.70, 1.60, 1.35, 0.90, 1.10,
    1.00, 0.75, 1.15, 1.15, 1.90,
    0.85, 0.95, 1.45, 1.70, 2.30,
    1.00, 0.70, 1.55, 1.40, 2.00
  });

  for (auto x = 0; x < expected->getWidth(); ++x) {
    for (auto y = 0; y < expected->getHeight(); ++y) {
      BOOST_CHECK_CLOSE(expected->getValue(x, y), image->getValue(x, y), 1e-4);
    }
  }
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()
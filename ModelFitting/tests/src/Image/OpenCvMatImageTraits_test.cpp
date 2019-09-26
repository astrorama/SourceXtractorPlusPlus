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
 * OpenCvMatImageTraits_test.cpp
 *
 *  Created on: May 23, 2019
 *      Author: mschefer
 */

#include <iostream>

#include <boost/test/unit_test.hpp>

#include "ModelFitting/Image/OpenCvMatImageTraits.h"

using namespace ModelFitting;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (OpenCvMatImageTraits_test)

//-----------------------------------------------------------------------------

using Traits = ImageTraits<cv::Mat>;

void copy_test(unsigned int image_size) {
  auto image_original = Traits::factory(image_size,image_size);
  auto image_copy = Traits::factory(image_size,image_size);

  for (unsigned int i=0; i < image_size; i++) {
    for (unsigned int j=0; j < image_size; j++) {
      Traits::at(image_original, i, j) = i * image_size + j;
      Traits::at(image_copy, i, j) = 0;
    }
  }

  Traits::addImageToImage(image_copy, image_original, 1, Traits::width(image_original) / 2.0, Traits::height(image_original) / 2.0);

  for (unsigned int i=0; i < image_size; i++) {
    for (unsigned int j=0; j < image_size; j++) {
      BOOST_CHECK_CLOSE(Traits::at(image_original, i, j), Traits::at(image_copy, i, j), .001);
    }
  }
}

BOOST_AUTO_TEST_CASE (copy_test_even) {
  copy_test(128);
}

BOOST_AUTO_TEST_CASE (copy_test_odd) {
  copy_test(127);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()

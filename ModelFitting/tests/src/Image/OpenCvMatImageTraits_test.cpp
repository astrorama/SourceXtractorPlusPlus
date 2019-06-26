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

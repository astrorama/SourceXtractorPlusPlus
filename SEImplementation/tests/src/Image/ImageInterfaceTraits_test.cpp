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
 * ImageInterfaceTraits_test.cpp
 *
 *  Created on: May 23, 2019
 *      Author: mschefer
 */

#include <iostream>
#include <numeric>

#include "SEUtils/IsNan.h"
#include "ModelFitting/Models/FrameModel.h"
#include "ModelFitting/Parameters/ManualParameter.h"
#include "SEImplementation/Image/DownSampledImagePsf.h"

#include <boost/test/unit_test.hpp>

#include "SEImplementation/Image/ImageInterfaceTraits.h"

using namespace SourceXtractor;
using namespace ModelFitting;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (ImageInterfaceTraits_test)

//-----------------------------------------------------------------------------

using Traits = ImageTraits<ImageInterfaceTypePtr>;


// The resampler treats samples outside the finite source image as zero.

void copy_test(unsigned int image_size, unsigned int check_border) {
  auto image_original = Traits::factory(image_size,image_size);
  auto image_copy = Traits::factory(image_size,image_size);

  for (unsigned int i=0; i < image_size; i++) {
    for (unsigned int j=0; j < image_size; j++) {
      Traits::at(image_original, i, j) = i * image_size + j;
      Traits::at(image_copy, i, j) = 0;
    }
  }

  Traits::addImageToImage(image_copy, image_original, 1, Traits::width(image_original) / 2.0, Traits::height(image_original) / 2.0);

  for (unsigned int i=check_border; i < image_size - check_border; i++) {
    for (unsigned int j=check_border; j < image_size - check_border; j++) {
      BOOST_CHECK_CLOSE(Traits::at(image_original, i, j), Traits::at(image_copy, i, j), .001);
    }
  }
}

BOOST_AUTO_TEST_CASE (copy_test_even) {
  copy_test(128, 4);
}

BOOST_AUTO_TEST_CASE (copy_test_odd) {
  copy_test(127, 4);
}


//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(small_images_copy_including_boundaries) {
  for (unsigned int size : {1, 3, 5, 7, 9}) {
    copy_test(size, 0);
  }
}

BOOST_AUTO_TEST_CASE(small_psfs_render_point_sources) {
  for (int size : {1, 3, 5, 7, 9}) {
    for (double down_scaling : {1.0, 0.5}) {
      auto kernel = Traits::factory(size, size);
      Traits::at(kernel, size / 2, size / 2) = 2;
      DownSampledImagePsf psf(1, kernel, down_scaling, false);
      auto scaled_kernel = psf.getScaledKernel(1);
      const auto kernel_sum = std::accumulate(scaled_kernel->getData().begin(), scaled_kernel->getData().end(), 0.0);
      BOOST_CHECK_CLOSE(kernel_sum, 2, 0.001);
      std::vector<PointModel> points;
      points.emplace_back(std::make_shared<ManualParameter>(5.5), std::make_shared<ManualParameter>(5.5),
          std::make_shared<ManualParameter>(1));
      FrameModel<DownSampledImagePsf, ImageInterfaceTypePtr> model(1, 11, 11, {}, std::move(points), {}, psf);
      auto image = model.getImage();
      for (auto pixel : image->getData()) {
        BOOST_CHECK(!fastmath_isnan(pixel));
        BOOST_CHECK(!fastmath_isinf(pixel));
      }
      const auto integral = std::accumulate(image->getData().begin(), image->getData().end(), 0.0);
      BOOST_CHECK_GT(integral, 0);
      if (down_scaling == 1) {
        BOOST_CHECK_CLOSE(integral, 2, 0.001);
      }
    }
  }
}

BOOST_AUTO_TEST_SUITE_END ()

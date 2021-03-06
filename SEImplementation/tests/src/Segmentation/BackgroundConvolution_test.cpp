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
#include "SEImplementation/Segmentation/BgConvolutionImageSource.h"
#include "SEImplementation/Segmentation/BgDFTConvolutionImageSource.h"
#include <boost/test/unit_test.hpp>
#include <boost/mpl/list.hpp>
#include <random>

#include "SEFramework/Image/ImageTile.h"

#include "SEUtils/TestUtils.h"

using namespace SourceXtractor;

BOOST_AUTO_TEST_SUITE (BackgroundConvolution_test)

//-----------------------------------------------------------------------------


//typedef boost::mpl::list<BgDFTConvolutionImageSource> convolution_types;
typedef boost::mpl::list<BgConvolutionImageSource, BgDFTConvolutionImageSource> convolution_types;

BOOST_AUTO_TEST_CASE_TEMPLATE (kernel3_background, T, convolution_types) {
  auto image = VectorImage<SeFloat>::create(
    5, 5, std::vector<SeFloat>{
      1.0, 2.0, 3.0, 4.0, 5.0,
      1.1, 2.1, 3.1, 4.1, 5.1,
      1.2, 2.2, 3.2, 4.2, 5.2,
      1.3, 2.3, 3.3, 4.3, 5.3,
      1.4, 2.4, 3.4, 4.4, 5.4,
    }
  );
  auto variance = VectorImage<SeFloat>::create(
    5, 5, std::vector<SeFloat>{
      1.0, 0.8, 0.7, 1.0, 0.6,
      1.0, 0.6, 0.8, 0.6, 1.0,
      1.0, 1.0, 0.1, 0.2, 1.0,
      1.0, 0.6, 0.8, 0.6, 1.0,
      1.0, 0.9, 1.0, 1.0, 1.0,
    }
  );
  auto kernel = VectorImage<SeFloat>::create(
    3, 3, std::vector<SeFloat>{
      0.0, 0.5, 0.0,
      0.5, 1.0, 0.5,
      0.0, 0.5, 0.0
    }
  );

  auto image_source = std::make_shared<T>(image, variance, 0.5, kernel);
  std::shared_ptr<ImageTile> tile = image_source->getImageTile(0, 0, 5, 5);
  auto result = std::dynamic_pointer_cast<ImageTileWithType<SeFloat>>(tile)->getImage();

  // When applying the kernel for getting the value of a cell, the convolution is done only for pixels
  // with a variance below the threshold: basically, on a masked image.
  // The convolved image is masked again, so pixels that are next to a pixel below the threshold remain 0.
  // For this test, there are only two cells below the threshold (0.1 and 0.2, corresponding to 3.2 and 4.2)
  auto expected = VectorImage<SeFloat>::create(
    5, 5, std::vector<SeFloat>{
      0.00, 0.00, 0.00, 0.00, 0.00,
      0.00, 0.00, 0.00, 0.00, 0.00,
      0.00, 0.00, 3.53333, 3.86667, 0.00,
      0.00, 0.00, 0.00, 0.00, 0.00,
      0.00, 0.00, 0.00, 0.00, 0.00,
    }
  );

  BOOST_CHECK(compareImages(expected, result));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE_TEMPLATE (kernel3_background_with_offset, T, convolution_types) {
  auto image = VectorImage<SeFloat>::create(
    5, 5, std::vector<SeFloat>{
      1.0, 2.0, 3.0, 4.0, 5.0,
      1.1, 2.1, 3.1, 4.1, 5.1,
      1.2, 2.2, 3.2, 4.2, 5.2,
      1.3, 2.3, 3.3, 4.3, 5.3,
      1.4, 2.4, 3.4, 4.4, 5.4,
    }
  );
  auto variance = VectorImage<SeFloat>::create(
    5, 5, std::vector<SeFloat>{
      1.0, 0.8, 0.7, 1.0, 0.6,
      1.0, 0.6, 0.8, 0.6, 1.0,
      1.0, 1.0, 0.1, 0.2, 1.0,
      1.0, 0.6, 0.8, 0.6, 1.0,
      1.0, 0.9, 1.0, 1.0, 1.0,
    }
  );
  auto kernel = VectorImage<SeFloat>::create(
    3, 3, std::vector<SeFloat>{
      0.0, 0.5, 0.0,
      0.5, 1.0, 0.5,
      0.0, 0.5, 0.0
    }
  );

  auto image_source = std::make_shared<T>(image, variance, 0.5, kernel);
  std::shared_ptr<ImageTile> tile = image_source->getImageTile(1, 1, 3, 4);
  auto result = std::dynamic_pointer_cast<ImageTileWithType<SeFloat>>(tile)->getImage();

  auto expected = VectorImage<SeFloat>::create(
    3, 4, std::vector<SeFloat>{
      0.00, 0.00, 0.00,
      0.00, 3.53333, 3.86667,
      0.00, 0.00, 0.00,
      0.00, 0.00, 0.00,
    }
  );

  BOOST_CHECK(compareImages(expected, result));
}

//-----------------------------------------------------------------------------

static std::shared_ptr<VectorImage<SeFloat>> generateImage(int size) {
  std::default_random_engine random_generator;
  std::uniform_real_distribution<SeFloat> random_dist{0, 1};

  auto img = VectorImage<SeFloat>::create(size, size);
  for (int x = 0; x < size; ++x) {
    for (int y = 0; y < size; ++y) {
      img->setValue(x, y, random_dist(random_generator));
    }
  }
  return img;
}

template <size_t n>
struct KernelSize {
  size_t getSize() const { return n; }
};

typedef boost::mpl::list<KernelSize<5>, KernelSize<7>, KernelSize<9>, KernelSize<11>> kernel_sizes;

BOOST_AUTO_TEST_CASE_TEMPLATE (kerneln_background, T, kernel_sizes) {
  // For a big kernel, just compare they both do the same
  // Doing it manually would be painful, but the previous test, with small sizes,
  // can be easily followed up by hand

  auto image = generateImage(128);
  auto variance = generateImage(128);
  auto kernel = generateImage(T().getSize());

  auto direct_source = std::make_shared<BgConvolutionImageSource>(image, variance, 0.5, kernel);
  auto dft_source = std::make_shared<BgDFTConvolutionImageSource>(image, variance, 0.5, kernel);

  std::shared_ptr<ImageTile> direct_tile = direct_source->getImageTile(0, 0, 128, 128);
  auto direct_result = std::dynamic_pointer_cast<ImageTileWithType<SeFloat>>(direct_tile)->getImage();


  std::shared_ptr<ImageTile> dft_tile = dft_source->getImageTile(0, 0, 128, 128);
  auto dft_result = std::dynamic_pointer_cast<ImageTileWithType<SeFloat>>(dft_tile)->getImage();

  BOOST_CHECK(compareImages(direct_result, dft_result, 1e-8, 1e-4));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()

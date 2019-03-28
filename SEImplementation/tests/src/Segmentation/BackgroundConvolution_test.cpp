#include "SEImplementation/Segmentation/BackgroundConvolution.h"
#include <boost/test/unit_test.hpp>
#include <boost/mpl/list.hpp>
#include <random>

#include "SEFramework/tests/src/Image/CompareImages.h"

using namespace SExtractor;

BOOST_AUTO_TEST_SUITE (BackgroundConvolution_test)

//-----------------------------------------------------------------------------

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
  auto result = image_source->getImageTile(0, 0, 5, 5)->getImage();

  // When applying the kernel for getting the value of a cell, is there is no value corresponding to a
  // variance below the threshold, the kernel is applied as one would normally expect.
  // Otherwise, *only* the values that are below the threshold are actually used for the given cell.
  // For this test, there are only two cells below the threshold (0.1 and 0.2, corresponding to 3.2 and 4.2)
  auto expected = VectorImage<SeFloat>::create(
    5, 5, std::vector<SeFloat>{
      1.275, 2.020, 3.02000, 4.02000, 4.775,
      1.300, 2.100, 3.20000, 4.20000, 4.900,
      1.400, 3.200, 3.53333, 3.86667, 4.200,
      1.500, 2.300, 3.20000, 4.20000, 5.100,
      1.625, 2.380, 3.38000, 4.38000, 5.125,
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
  auto result = image_source->getImageTile(1, 1, 3, 4)->getImage();

  auto expected = VectorImage<SeFloat>::create(
    3, 4, std::vector<SeFloat>{
      2.100, 3.20000, 4.20000,
      3.200, 3.53333, 3.86667,
      2.300, 3.20000, 4.20000,
      2.380, 3.38000, 4.38000,
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
  // For a big kernel, just compare both do the same
  // Doing it manually would be painful, but the previous test, with small sizes,
  // can be easily followed up by hand

  auto image = generateImage(128);
  auto variance = generateImage(128);
  auto kernel = generateImage(T().getSize());

  auto direct_source = std::make_shared<BgConvolutionImageSource>(image, variance, 0.5, kernel);
  auto dft_source = std::make_shared<BgDFTConvolutionImageSource>(image, variance, 0.5, kernel);

  auto direct_result = direct_source->getImageTile(0, 0, 128, 128)->getImage();
  auto dft_result = dft_source->getImageTile(0, 0, 128, 128)->getImage();

  BOOST_CHECK(compareImages(direct_result, dft_result, 1e-8, 1e-4));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()

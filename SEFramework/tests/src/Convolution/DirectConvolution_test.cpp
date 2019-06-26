//
// Created by aalvarez on 8/17/18.
//

#include <boost/test/unit_test.hpp>
#include <ElementsKernel/Real.h>
#include "SEFramework/Convolution/DirectConvolution.h"

using namespace SExtractor;

struct DirectConvolution_Fixture {
  DirectConvolution<SeFloat, PaddedImage<SeFloat>> direct;

  DirectConvolution_Fixture() : direct{VectorImage<SeFloat>::create(
    3, 3,
    std::vector<SeFloat>{
      1, 2, 3,
      4, 5, 6,
      7, 8, 9
    })} {
  }
};

BOOST_AUTO_TEST_SUITE (DirectConvolution_test)

BOOST_FIXTURE_TEST_CASE ( Convolve_test, DirectConvolution_Fixture ) {
  auto image = VectorImage<SeFloat>::create(5, 5, std::vector<SeFloat>{
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 1.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
  });


  direct.convolve(image);

  auto expected = VectorImage<SeFloat>::create(5, 5, std::vector<SeFloat>{
    1.0, 2.0, 3.0, 0.0, 0.0,
    4.0, 5.0, 6.0, 0.0, 0.0,
    7.0, 8.0, 9.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
  });

  for (auto x = 0; x < expected->getWidth(); ++x) {
    for (auto y = 0; y < expected->getHeight(); ++y) {
      auto ev = expected->getValue(x, y);
      auto iv = image->getValue(x, y);
      if (Elements::isNotEqual(ev, iv)) {
        BOOST_ERROR("Mismatch at " << x << 'x' << y << ": " << ev << " != " << iv);
      }
    }
  }
}

BOOST_FIXTURE_TEST_CASE ( Convolve_not_squared_test, DirectConvolution_Fixture ) {
  auto image = VectorImage<SeFloat>::create(5, 3, std::vector<SeFloat>{
    0.0, 0.5, 0.0, 0.0, 0.0,
    0.0, 0.0, 1.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
  });


  direct.convolve(image);

  auto expected = VectorImage<SeFloat>::create(5, 3, std::vector<SeFloat>{
    2.0,  3.5,  5.0, 3.0, 0.0,
    3.5,  8.0,  9.5, 6.0, 0.0,
    0.0,  7.0,  8.0, 9.0, 0.0,
  });

  for (auto x = 0; x < expected->getWidth(); ++x) {
    for (auto y = 0; y < expected->getHeight(); ++y) {
      auto ev = expected->getValue(x, y);
      auto iv = image->getValue(x, y);
      if (std::abs(ev - iv) > std::numeric_limits<float>::epsilon()) {
        BOOST_ERROR("Mismatch at " << x << 'x' << y << ": " << ev << " != " << iv);
      }
    }
  }
}

BOOST_AUTO_TEST_SUITE_END ()

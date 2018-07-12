/*
 * VariablePsf.h
 *
 *  Created on: Jun 25, 2018
 *      Author: Alejandro Álvarez Ayllón
 */

#include <boost/test/unit_test.hpp>
#include <SEImplementation/Plugin/Psf/PsfProperty.h>
#include <SEFramework/Source/SimpleSourceGroup.h>
#include <SEImplementation/Plugin/DetectionFrameGroupStamp/DetectionFrameGroupStamp.h>
#include <numeric>
#include "SEImplementation/Plugin/Psf/PsfTask.h"

using namespace SExtractor;

void checkEqual(const std::shared_ptr<VectorImage<SeFloat>> &a, const  std::shared_ptr<VectorImage<SeFloat>> &b) {
  BOOST_REQUIRE_EQUAL(a->getWidth(), b->getWidth());
  BOOST_REQUIRE_EQUAL(a->getHeight(), b->getWidth());

  for (auto i = 0; i < a->getWidth(); ++i) {
    for (auto j = 0; j < a->getHeight(); ++j) {
      // We assume the PSF summed value will normally be around O(1)
      // so the error should remain bellow float::epsilon
      BOOST_CHECK_LE(std::abs(a->at(i, j) - b->at(i, j)), std::numeric_limits<float>::epsilon());
    }
  }
}

auto constant = VectorImage<SeFloat>::create(3, 3, std::vector<SeFloat>{
    0. , 1., 0.,
    0.5, 1., 0.5,
    0. , 1., 0.
});
auto x = VectorImage<SeFloat>::create(3, 3, std::vector<SeFloat>{
    0., 0., 0.,
    0., 2., 0.,
    0., 0., 0.
});
auto y = VectorImage<SeFloat>::create(3, 3, std::vector<SeFloat>{
    1., 0., 0.,
    0., 0., 0.,
    0., 0., 0.2
});

struct VariablePsfFixture {
  SimpleSourceGroup group;
  std::shared_ptr<VariablePsf> varPsf{new VariablePsf(
    1., std::vector<VariablePsf::Component>{{"X_IMAGE", 0, 5., 2.}, {"Y_IMAGE", 0, 20., 30.}},
    {1}, {constant, x, y}
  )};
  PsfTask varPsfTask{0, varPsf};
};


BOOST_AUTO_TEST_SUITE (VariablePsfProperty_test)

BOOST_FIXTURE_TEST_CASE (variable_psf_simple, VariablePsfFixture) {
  auto expected = VectorImage<SeFloat>::create(3, 3, std::vector<SeFloat>{
      1. , 1., 0.,
      0.5, 4., 0.5,
      0. , 1., 0.2
  });
  auto expected_sum = std::accumulate(expected->getData().begin(), expected->getData().end(), 0.);
  for (auto i = 0; i < expected->getWidth(); ++i) {
    for (auto j = 0; j < expected->getHeight(); ++j) {
      expected->setValue(i, j, expected->getValue(i, j) / expected_sum);
    }
  }

  auto mock_stamp = VectorImage<SeFloat>::create(8. * 2, 50. * 2);
  group.setProperty<DetectionFrameGroupStamp>(mock_stamp, nullptr, PixelCoordinate{0, 0}, nullptr);

  varPsfTask.computeProperties(group);
  auto psf_prop = group.getProperty<PsfProperty>();
  checkEqual(psf_prop.getPsf().getScaledKernel(1), expected);
}

BOOST_AUTO_TEST_SUITE_END ()

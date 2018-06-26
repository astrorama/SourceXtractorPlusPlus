/*
 * VariablePsf.h
 *
 *  Created on: Jun 25, 2018
 *      Author: Alejandro Álvarez Ayllón
 */

#include <boost/test/unit_test.hpp>
#include <SEFramework/Source/SimpleSource.h>
#include <SEImplementation/Plugin/PixelCentroid/PixelCentroid.h>
#include <SEImplementation/Plugin/VariablePsf/VariablePsfProperty.h>
#include "SEImplementation/Plugin/VariablePsf/VariablePsfTask.h"

using namespace SExtractor;

void checkEqual(const std::shared_ptr<VectorImage<double>> &a, const  std::shared_ptr<VectorImage<double>> &b) {
  BOOST_CHECK_EQUAL(a->getWidth(), b->getWidth());
  BOOST_CHECK_EQUAL(a->getHeight(), b->getWidth());

  for (auto i = 0; i < a->getWidth(); ++i) {
    for (auto j = 0; j < a->getHeight(); ++j) {
      BOOST_REQUIRE_CLOSE(a->at(i, j), b->at(i, j), 0.01);
    }
  }
}

auto constant = VectorImage<double>::create(3, 3, std::vector<double>{
    0. , 1., 0.,
    0.5, 1., 0.5,
    0. , 1., 0.
});
auto x = VectorImage<double>::create(3, 3, std::vector<double>{
    0., 0., 0.,
    0., 2., 0.,
    0., 0., 0.
});
auto y = VectorImage<double>::create(3, 3, std::vector<double>{
    1., 0., 0.,
    0., 0., 0.,
    0., 0., 0.2
});

struct VariablePsfFixture {
  SimpleSource source;
  VariablePsf varPsf{{{"x", 0, 5., 2.}, {"y", 0, 20., 30.}}, {1}, {constant, x, y}};
  VariablePsfTask varPsfTask{varPsf};
};


BOOST_AUTO_TEST_SUITE (VariablePsfProperty_test)

BOOST_FIXTURE_TEST_CASE (variable_psf_simple, VariablePsfFixture) {
  const auto expected = VectorImage<double>::create(3, 3, std::vector<double>{
      1. , 1., 0.,
      0.5, 4., 0.5,
      0. , 1., 0.2
  });

  source.setProperty<PixelCentroid>(8., 50.);

  varPsfTask.computeProperties(source);
  auto psf_prop = source.getProperty<VariablePsfProperty>();
  checkEqual(psf_prop.getPsf(), expected);
}

BOOST_AUTO_TEST_SUITE_END ()

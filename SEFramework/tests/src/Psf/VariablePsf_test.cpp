/**
 * @file tests/src/Psf/VariablePsf.cpp
 * @date 25/06/18
 * @author Alejandro Álvarez Ayllón
 */

#include <boost/test/unit_test.hpp>
#include <iostream>
#include <ElementsKernel/Exception.h>
#include "SEFramework/Psf/VariablePsf.h"

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
auto x2 = VectorImage<double>::create(3, 3, std::vector<double>{
    1., 0., 0.,
    0., 0., 0.,
    0., 0., 0.2
});


BOOST_AUTO_TEST_SUITE (VariablePsf_test)

/// Can not create an empty variable PSF
BOOST_AUTO_TEST_CASE (malformed_empty) {
  try {
    VariablePsf psf{{}, {}, {}};
    BOOST_FAIL("Creation of an empty variable PSF must fail");
  }
  catch (const Elements::Exception&) {
  }
}

/// Not all coefficient matrices have the proper dimensionality
BOOST_AUTO_TEST_CASE (malformed_coeff_dimensions) {
  auto bad = VectorImage<double>::create(2, 2, std::vector<double>{
      1., 2.,
      3., 1.,
  });

  try {
    VariablePsf psf{{}, {}, {constant, bad}};
    BOOST_FAIL("Creation of variable PSF with mismatching coefficient dimensions");
  }
  catch (const Elements::Exception&) {
  }
}

/// There is only a constant coefficient, so the coordinates must not matter
BOOST_AUTO_TEST_CASE (constant_only) {
  VariablePsf varPsf{{}, {}, {constant}};

  BOOST_CHECK_EQUAL(varPsf.getWidth(), 3);
  BOOST_CHECK_EQUAL(varPsf.getHeight(), 3);

  auto psf = varPsf.getPsf({});
  checkEqual(psf, constant);
}

BOOST_AUTO_TEST_CASE (x_linear) {
  const auto expected = VectorImage<double>::create(3, 3, std::vector<double>{
      0. , 1., 0.,
      0.5, 4., 0.5,
      0. , 1., 0.
  });

  VariablePsf varPsf{{{"x", 0, 5., 2.}}, {1}, {constant, x}};

  auto psf = varPsf.getPsf({8.});
  checkEqual(psf, expected);
}

BOOST_AUTO_TEST_CASE (x_squared) {
  const auto expected = VectorImage<double>::create(3, 3, std::vector<double>{
      2.25, 1., 0.,
      0.5 , 4., 0.5,
      0.  , 1., 0.45
  });

  VariablePsf varPsf{{{"x", 0, 5, 2}}, {2}, {constant, x, x2}};

  auto psf = varPsf.getPsf({8.});
  checkEqual(psf, expected);
}

BOOST_AUTO_TEST_CASE(x_y_linear) {
  const auto expected = VectorImage<double>::create(3, 3, std::vector<double>{
      1. , 1., 0.,
      0.5, 4., 0.5,
      0. , 1., 0.2
  });

  VariablePsf varPsf{{{"x", 0, 5., 2.}, {"y", 0, 20., 30.}}, {1}, {constant, x, x2}};

  auto psf = varPsf.getPsf({8., 50.});
  checkEqual(psf, expected);
}

BOOST_AUTO_TEST_CASE(x_y_squared) {
  const auto expected = VectorImage<double>::create(3, 3, std::vector<double>{
      3.75, 1., 0.,
      0.5 , 8., 0.5,
      0.  , 1., 0.75
  });

  VariablePsf varPsf{{{"x", 0, 5., 2.}, {"y", 0, 20., 30.}}, {2}, {constant, x, x2, x, x2, x}};

  auto psf = varPsf.getPsf({8., 50.});
  checkEqual(psf, expected);
}


BOOST_AUTO_TEST_SUITE_END ()

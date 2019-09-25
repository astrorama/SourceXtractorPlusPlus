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
/**
 * @file tests/src/Psf/VariablePsf.cpp
 * @date 25/06/18
 * @author Alejandro Álvarez Ayllón
 */

#include <boost/test/unit_test.hpp>
#include <ElementsKernel/Exception.h>
#include "SEFramework/Psf/VariablePsf.h"

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
auto x2 = VectorImage<SeFloat>::create(3, 3, std::vector<SeFloat>{
    1., 0., 0.,
    0., 0., 0.,
    0., 0., 0.2
});
auto y = VectorImage<SeFloat>::create(3, 3, std::vector<SeFloat>{
    0. , 0., 0.,
    0. , 0., 0.,
    0.5, 0., 0.
});
auto xy = VectorImage<SeFloat>::create(3, 3, std::vector<SeFloat>{
    0.5, 0. , 0.,
    0. , 0. , 0.,
    0. , 0.5, 0.
});
auto x2y = VectorImage<SeFloat>::create(3, 3, std::vector<SeFloat>{
    0., 0., 0.,
    0., 0., 0.,
    0., 0., 1.
});


BOOST_AUTO_TEST_SUITE (VariablePsf_test)

/// Can not create an empty variable PSF
BOOST_AUTO_TEST_CASE (malformed_empty) {
  try {
    VariablePsf psf{1, {}, {}, {}};
    BOOST_FAIL("Creation of an empty variable PSF must fail");
  }
  catch (const Elements::Exception&) {
  }
}

/// Not all coefficient matrices have the proper dimensionality
BOOST_AUTO_TEST_CASE (malformed_coeff_dimensions) {
  auto bad = VectorImage<SeFloat>::create(2, 2, std::vector<SeFloat>{
      1., 2.,
      3., 1.,
  });

  try {
    VariablePsf psf{1, {}, {}, {constant, bad}};
    BOOST_FAIL("Creation of variable PSF with mismatching coefficient dimensions");
  }
  catch (const Elements::Exception&) {
  }
}

/// Missing coefficients
BOOST_AUTO_TEST_CASE (malformed_not_enough_coeff) {
  try {
    VariablePsf varPsf{1, {{"x", 0, 5, 2}}, {2}, {constant, x}};
    BOOST_FAIL("Creation of variable PSF with not enough coefficients");
  }
  catch (const Elements::Exception&) {
  }
}

/// Too many coefficients
BOOST_AUTO_TEST_CASE (malformed_too_many_coeff) {
  try {
    VariablePsf varPsf{1, {{"x", 0, 5, 2}}, {2}, {constant, x, x2, y}};
    BOOST_FAIL("Creation of variable PSF with too many coefficients");
  }
  catch (const Elements::Exception&) {
  }
}

/// There is only a constant coefficient, so the coordinates must not matter
BOOST_AUTO_TEST_CASE (constant_only) {
  VariablePsf varPsf{1, constant};

  BOOST_CHECK_EQUAL(varPsf.getWidth(), 3);
  BOOST_CHECK_EQUAL(varPsf.getHeight(), 3);

  auto psf = varPsf.getPsf({});
  checkEqual(psf, constant);
}

BOOST_AUTO_TEST_CASE (x_linear) {
  const auto expected = VectorImage<SeFloat>::create(3, 3, std::vector<SeFloat>{
      0. , 1., 0.,
      0.5, 4., 0.5,
      0. , 1., 0.
  });

  VariablePsf varPsf{1, {{"x", 0, 5., 2.}}, {1}, {constant, x}};

  auto psf = varPsf.getPsf({8.});
  checkEqual(psf, expected);
}

BOOST_AUTO_TEST_CASE (x_squared) {
  const auto expected = VectorImage<SeFloat>::create(3, 3, std::vector<SeFloat>{
      2.25, 1., 0.,
      0.5 , 4., 0.5,
      0.  , 1., 0.45
  });

  VariablePsf varPsf{1, {{"x", 0, 5, 2}}, {2}, {constant, x, x2}};

  auto psf = varPsf.getPsf({8.});
  checkEqual(psf, expected);
}

BOOST_AUTO_TEST_CASE(x_y_linear) {
  const auto expected = VectorImage<SeFloat>::create(3, 3, std::vector<SeFloat>{
      1. , 1., 0.,
      0.5, 4., 0.5,
      0. , 1., 0.2
  });

  VariablePsf varPsf{1, {{"x", 0, 5., 2.}, {"y", 0, 20., 30.}}, {1}, {constant, x, x2}};

  auto psf = varPsf.getPsf({8., 50.});
  checkEqual(psf, expected);
}

BOOST_AUTO_TEST_CASE(x_y_squared) {
  const auto expected = VectorImage<SeFloat>::create(3, 3, std::vector<SeFloat>{
      3.75, 1., 0.,
      0.5 , 8., 0.5,
      0.  , 1., 0.75
  });

  VariablePsf varPsf{1, {{"x", 0, 5., 2.}, {"y", 0, 20., 30.}}, {2}, {constant, x, x2, x, x2, x}};

  auto psf = varPsf.getPsf({8., 50.});
  checkEqual(psf, expected);
}

BOOST_AUTO_TEST_CASE(x_y_two_groups) {
  const auto expected = VectorImage<SeFloat>::create(3, 3, std::vector<SeFloat>{
      105.,  1.,   0.,
      0.5 , 21.,   0.5,
      0.5 ,  6., 120.
  });

  VariablePsf varPsf{1, {{"x", 0, 50., 5.}, {"y", 1, 20., 3.}}, {2, 1}, {constant, x, x2, y, xy, x2y}};

  auto psf = varPsf.getPsf({100., 23.});
  checkEqual(psf, expected);
}

// Previous tests are easy to reason about, and even can be verified with pen and paper.
// However, they are small (3x3) and with "simple" values
// This one uses a linear variable PSF extracted from a real PsfEx file,
// so it is more complex (25x25), and imply moving around the limits of the numbers that can
// be represented by a float.
// Note that the expected value was computed with a Python prototype that was validated
// against the PSF snapshot created by PsfEx, so this kind of expects the validation to be
// commutative :)
BOOST_AUTO_TEST_CASE(x_y_linear_fractional) {
  #include "LinearFullPsf.icpp"

  VariablePsf varPsf{1, {
        {"x", 0, 2040.414154053, 2924.028137207},
        {"y", 0, 1962.621307373, 3297.873596191}
    }, {1}, {full_constant, full_x, full_y}
  };

  auto psf = varPsf.getPsf({772.9703369140625, 1.0});
  checkEqual(psf, full_expected);
}

// This one is yet more complicated.
// It has been extracted from a PsfEx file, 45x45, and the degree is 3
BOOST_AUTO_TEST_CASE(x_y_cubic_fractional) {
  #include "CubicFullPsf.icpp"

  VariablePsf varPsf{1, {
      {"x", 0, 1896.602607727, 3534.829940796},
      {"y", 0, 1926.288711548, 3593.864959717}
  }, {3}, cubic_coefficients};

  auto psf = varPsf.getPsf({772.9703369140625, 1.0});
  checkEqual(psf, cubic_expected);
}

BOOST_AUTO_TEST_SUITE_END ()

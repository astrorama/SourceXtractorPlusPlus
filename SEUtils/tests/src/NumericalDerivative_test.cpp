/*
 * NumericalDerivative_test.cpp
 *
 *  Created on: Sep 17, 2019
 *      Author: mschefer
 */
#include <boost/test/unit_test.hpp>

#include <boost/version.hpp>
#if BOOST_VERSION >= 106600

#if BOOST_VERSION >= 107000
#include <boost/math/differentiation/finite_difference.hpp>
namespace bmd = boost::math::differentiation;
#else
#include <boost/math/tools/numerical_differentiation.hpp>
namespace bmd = boost::math::tools;
#endif

#endif

#include "SEUtils/NumericalDerivative.h"

using namespace SExtractor;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (NumericalDerivative_test)

//-----------------------------------------------------------------------------

struct TestFunction {
  std::function<double(double)> m_function;
  std::function<double(double)> m_analytical_derivative;
  double m_min, m_max, m_step;
};

namespace {
  TestFunction functions[] = {
      {
        [](double x) { return std::sin(x); },
        [](double x) { return std::cos(x); },
        -100.,
        100.,
        .1
      },
      {
        [](double x) { return x * x; },
        [](double x) { return 2.* x; },
        -10000.,
        10000.,
        10.
      },
      {
        [](double x) { return std::exp(x); },
        [](double x) { return std::exp(x); },
        -100.,
        100.,
        .1
      }
  };
}

BOOST_AUTO_TEST_CASE( central_difference_test ) {
  for (auto& function : functions) {
    for (double x = function.m_min; x <= function.m_max; x+=function.m_step) {
      BOOST_CHECK_CLOSE(NumericalDerivative::centralDifference(function.m_function, x),
          function.m_analytical_derivative(x), 0.0001);
    }
  }
}

#if BOOST_VERSION >= 106700
  BOOST_AUTO_TEST_CASE( boost_finite_difference_test ) {
    for (auto& function : functions) {
      for (double x = function.m_min; x <= function.m_max; x+=function.m_step) {
        BOOST_CHECK_CLOSE(bmd::finite_difference_derivative(function.m_function, x),
            function.m_analytical_derivative(x), 0.0001);
      }
    }
  }
#endif

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()

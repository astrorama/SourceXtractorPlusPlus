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

#include <boost/test/unit_test.hpp>
#include "SEImplementation/Background/BackgroundHistogram.h"
#include "SEUtils/TestUtils.h"

using namespace SourceXtractor;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (BackgroundHistogram_test)

struct Histogram_fixture {
  double mean  = 177.145;
  double sigma = 213.350;
  // A uniform distribution between 0 and 100, plus a Gaussian centered on 2,2
  // with a size of 3 pixels, mean of 500 and sigma of 10
  std::vector<PIXTYPE> values{
     86.448,  2.937,  39.852,  99.787,  52.619, 31.394,
     82.123, 16.726,  38.581,  79.113,  18.728,  3.696,
     45.243, 36.473, 512.402, 533.707, 514.845, 44.367,
     24.502, 97.271, 577.627, 524.799, 586.751, 91.262,
     87.275, 12.312, 561.586, 511.300, 562.612, 95.877,
     95.026, 43.359,  40.713,  53.100,  98.101, 74.702};
};

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(basic_test, Histogram_fixture) {
  BackgroundHistogram histogram{mean, sigma, values.size()};
  BOOST_CHECK_EQUAL(mean, histogram.itsMean);
  BOOST_CHECK_EQUAL(sigma, histogram.itsSigma);

  std::for_each(values.begin(), values.end(), [&histogram](PIXTYPE v) { histogram.addDatum(v); });

  PIXTYPE backVal, sigmaVal;
  histogram.getBackGuess(backVal, sigmaVal);
  BOOST_CHECK(checkIsClose(76.396, backVal));
  BOOST_CHECK(checkIsClose(217.020, sigmaVal));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()

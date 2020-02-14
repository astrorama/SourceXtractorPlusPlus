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
#include "SEImplementation/Background/SE/KappaSigmaBinning.h"

using namespace SourceXtractor;

struct KappaSigmaBinningFixture {
  // Generated with a Gaussian distribution centered at 10 and with a standard deviation of 1
  float mean = 10, sigma = 1;
  std::vector<float> data{11.76407119,  8.61457818, 11.4649462 , 10.75417767,  9.5009931 ,
                          9.97867712, 11.34706882, 10.46950448, 10.69265133, 10.78622591,
                          10.0036409 ,  9.32309404,  9.57130063,  9.97337101,  9.32753176,
                          10.25151267,  8.73039703,  9.67309398, 10.21744849,  9.32599956};
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(KappaSigmaBinning_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(nsigma5, KappaSigmaBinningFixture) {
  KappaSigmaBinning binning(1, 5);
  auto edges = binning(data.begin(), data.end());

  BOOST_CHECK_EQUAL(16, edges.size());
  BOOST_CHECK(std::is_sorted(edges.begin(), edges.end()));
  BOOST_CHECK_GE(edges.front(), mean - 5 * sigma);
  BOOST_CHECK_LT(edges.back(), mean + 5 * sigma);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(nsigma1, KappaSigmaBinningFixture) {
  KappaSigmaBinning binning(1, 1);
  auto edges = binning(data.begin(), data.end());

  BOOST_CHECK_EQUAL(4, edges.size());
  BOOST_CHECK(std::is_sorted(edges.begin(), edges.end()));
  BOOST_CHECK_GE(edges.front(), mean - sigma);
  BOOST_CHECK_LT(edges.back(), mean + sigma);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(allZeros) {
  std::vector<float> zeros{0, 0, 0, 0};
  KappaSigmaBinning binning(1, 1);
  auto edges = binning(zeros.begin(), zeros.end());
  BOOST_CHECK_EQUAL(2, edges.size());
  BOOST_CHECK_LT(edges.front(), 0);
  BOOST_CHECK_GT(edges.back(), 0);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(allOnes) {
  std::vector<float> ones{1, 1, 1, 1};
  KappaSigmaBinning binning(1, 1);
  auto edges = binning(ones.begin(), ones.end());
  BOOST_CHECK_EQUAL(2, edges.size());
  BOOST_CHECK_LT(edges.front(), 1);
  BOOST_CHECK_GT(edges.back(), 1);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()

//-----------------------------------------------------------------------------


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
#include "Histogram/Histogram.h"
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
  KappaSigmaBinning<float> binning(1, 5);
  binning.computeBins(data.begin(), data.end());
  auto edges = binning.getEdges();

  BOOST_CHECK_EQUAL(16, edges.size());
  BOOST_CHECK(std::is_sorted(edges.begin(), edges.end()));
  BOOST_CHECK_GE(edges.front(), mean - 5 * sigma);
  BOOST_CHECK_LT(edges.back(), mean + 5 * sigma);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(nsigma1, KappaSigmaBinningFixture) {
  KappaSigmaBinning<float> binning(1, 1);
  binning.computeBins(data.begin(), data.end());
  auto edges = binning.getEdges();

  BOOST_CHECK_EQUAL(4, edges.size());
  BOOST_CHECK(std::is_sorted(edges.begin(), edges.end()));
  BOOST_CHECK_GE(edges.front(), mean - sigma);
  BOOST_CHECK_LT(edges.back(), mean + sigma);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(allZeros) {
  std::vector<float> zeros{0, 0, 0, 0};
  KappaSigmaBinning<float> binning(1, 1);
  binning.computeBins(zeros.begin(), zeros.end());
  auto edges = binning.getEdges();
  BOOST_CHECK_EQUAL(2, edges.size());
  BOOST_CHECK_LT(edges.front(), 0);
  BOOST_CHECK_GT(edges.back(), 0);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(allOnes) {
  std::vector<float> ones{1, 1, 1, 1};
  KappaSigmaBinning<float> binning(1, 1);
  binning.computeBins(ones.begin(), ones.end());
  auto edges = binning.getEdges();
  BOOST_CHECK_EQUAL(2, edges.size());
  BOOST_CHECK_LT(edges.front(), 1);
  BOOST_CHECK_GT(edges.back(), 1);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(allOnesHistogram) {
  std::vector<float> ones{1, 1, 1, 1};

  Euclid::Histogram::Histogram<float> histo(ones.begin(), ones.end(), KappaSigmaBinning<float>{});
  auto edges = histo.getEdges();
  auto counts = histo.getCounts();
  float total = 0;
  for (size_t i = 0; i < counts.size(); ++i) {
    if (edges[i] <= 1. && edges[i+1] >= 1) {
      total += counts[i];
    }
  }
  BOOST_CHECK_EQUAL(4.f, total);

  float mean, median, sigma;
  std::tie(mean, median, sigma) = histo.getStats();
  BOOST_CHECK_CLOSE(1.f, mean, 1e-2);
  BOOST_CHECK_CLOSE(1.f, median, 1e-2);
  BOOST_CHECK_SMALL(sigma, 1e-5f);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()

//-----------------------------------------------------------------------------


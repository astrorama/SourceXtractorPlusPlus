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
  std::vector<float> data{11.76407119, 8.61457818, 11.4649462, 10.75417767, 9.5009931,
                          9.97867712, 11.34706882, 10.46950448, 10.69265133, 10.78622591,
                          10.0036409, 9.32309404, 9.57130063, 9.97337101, 9.32753176,
                          10.25151267, 8.73039703, 9.67309398, 10.21744849, 9.32599956};
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(KappaSigmaBinning_test)

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
    if (edges[i] <= 1. && edges[i + 1] >= 1) {
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
// Verify that the histogram result matches the implementation before refactoring
//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(compatTest) {
  std::vector<float> values{
    4857.2197, 7628.263, 12726.14, 21709.928, 60000., 60000., 60000., 60000., 60000., 60000.,
    3991.6655, 5723.0806, 8843.776, 14066.48, 60000., 60000., 60000., 60000., 60000., 60000.,
    3248.9814, 4297.6294, 6044.9634, 8816.455, 29915.367, 60000., 60000., 60000., 60000., 60000.,
    2583.1348, 3472.0464, 4294.44, 5713.629, 7843.184, 60000., 60000., 60000., 60000., 60000.,
    2245.285, 2621.3354, 3225.143, 3936.3076, 4787.384, 60000., 60000., 60000., 60000., 60000.,
    1887.6696, 2060.921, 2497.0254, 2788.3164, 3315.3152, 60000., 60000., 60000., 60000., 60000.,
    1607.623, 1784.2932, 1956.1333, 2262.085, 2353.2268, 31473.373, 60000., 60000., 60000., 60000.,
    1416.6884, 1585.911, 1648.5911, 1734.5569, 1944.6141, 2025.1901, 60000., 60000., 60000., 60000.,
    1312.7026, 1300.9614, 1425.7368, 1545.1123, 1574.7478, 1601.7433, 60000., 60000., 60000., 60000.,
    1135.4432, 1153.5099, 1214.0371, 1263.719, 1224.6412, 1280.733, 60000., 60000., 60000., 60000.};

  std::vector<float> expected{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    21, 18, 4, 4, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 48, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  Euclid::Histogram::Histogram<double> histo(values.begin(), values.end(), KappaSigmaBinning<double>{});
  auto result = histo.getCounts();
  BOOST_CHECK_EQUAL_COLLECTIONS(expected.begin(), expected.end(), result.begin(), result.end());

  float mean, median, sigma;
  std::tie(mean, median, sigma) = histo.getStats();

  BOOST_CHECK_CLOSE(27666.56, sigma, 1e-2);
  BOOST_CHECK_CLOSE(30882.854, mean, 1e-2);
  BOOST_CHECK_CLOSE(24347.632, median, 1e-2);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(compat2Test) {
  std::vector<float> values{
    60000., 60000., 58254.043, 31667.43, 17265.59, 9496.521, 5579.286, 3418.8032, 2425.4026, 1798.5122,
    60000., 60000., 45827.15, 26480.13, 14519.701, 8105.3887, 4956.8438, 3146.938, 2266.9792, 1733.0563,
    60000., 60000., 34206.965, 20255.967, 11735.001, 6850.9517, 4288.0464, 2922.809, 2150.1013, 1635.3331,
    60000., 60000., 23105.559, 14198.465, 8700.73, 5406.736, 3565.0156, 2567.0537, 1922.2461, 1590.5314,
    60000., 60000., 14979.693, 9847.7705, 6193.437, 4147.724, 3052.4558, 2269.6167, 1807.0782, 1496.3268,
    60000., 33484.64, 9271.388, 6580.7227, 4557.2227, 3349.1536, 2507.1213, 2062.0576, 1653.0007, 1424.8165,
    60000., 7733.7676, 5772.313, 4430.3384, 3399.7214, 2624.8774, 2091.131, 1707.4598, 1572.8206, 1364.9684,
    60000., 4871.3936, 3991.836, 3306.3687, 2615.8513, 2190.5598, 1878.2047, 1554.4739, 1477.5591, 1273.6274,
    52947.656, 3261.7979, 2696.0112, 2416.4526, 2039.2615, 1793.5526, 1551.5315, 1448.4581, 1279.2971, 1199.466,
    2685.9897, 2399.3604, 2132.107, 1869.6641, 1767.9985, 1586.2705, 1474.42, 1342.4452, 1246.5488, 1185.6676};

  std::vector<float> expected{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    15, 30, 9, 7, 3, 3, 2, 3, 1, 1, 0, 0, 3, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 1, 0, 0, 0, 0, 0, 1, 0,
  };

  Euclid::Histogram::Histogram<double> histo(values.begin(), values.end(), KappaSigmaBinning<double>{});
  auto result = histo.getCounts();
  BOOST_CHECK_EQUAL_COLLECTIONS(expected.begin(), expected.end(), result.begin(), result.end());

  float mean, median, sigma;
  std::tie(mean, median, sigma) = histo.getStats();

  BOOST_CHECK_CLOSE(9783.018924, sigma, 1e-2);
  BOOST_CHECK_CLOSE(6616.122366, mean, 1e-2);
  BOOST_CHECK_CLOSE(2638.740885, median, 1e-2);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(compat3Test) {
  std::vector<float> values{
    26.476217, 62.46932, 96.802345, 135.45041, 170.27917, 197.26395, 162.59398, 94.8131, 88.061935, 26.492489,
    58.84676, 115.30665, 170.3709, 243.85493, 304.7038, 276.17514, 195.66293, 123.31482, 84.60098, 51.17417,
    42.554115, 100.49637, 217.80646, 323.2457, 443.3379, 443.36133, 398.88367, 265.2297, 186.47343, 111.03885,
    55.871567, 123.68535, 189.38126, 296.32098, 423.5625, 503.601, 501.65103, 372.1471, 245.96036, 109.69481,
    82.07245, 106.721664, 130.33661, 232.66516, 362.95203, 487.74057, 518.54236, 396.96597, 248.10919, 149.03497,
    28.600458, 47.334534, 80.98803, 168.63583, 210.75912, 313.4767, 407.1878, 354.30994, 247.27121, 164.22421,
    33.597324, 59.817535, 40.67008, 79.29457, 119.81137, 184.209, 228.75455, 221.4667, 195.83687, 126.82794,
    48.596306, 31.861345, 48.346264, 78.50487, 82.579605, 83.018524, 138.96829, 132.50502, 108.37274, 86.83896,
    31.525652, -11.022741, 8.3624325, 35.238808, 45.804752, 26.984922, 69.14417, 65.99637, 28.278303, 31.00513,
    30.771547, 38.74517, 32.5546, 29.698442, 21.669147, 6.6021123, 25.072222, 49.178627, 17.329887, 32.05815
  };

  std::vector<float> expected{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 8,
    10, 7, 4, 2, 7, 4, 3, 4, 4, 3, 1, 2, 3, 2, 4, 0, 2, 2, 2, 3, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 2, 1, 1, 0, 2, 0,
    0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  };

  Euclid::Histogram::Histogram<double> histo(values.begin(), values.end(), KappaSigmaBinning<double>{});
  auto result = histo.getCounts();
  BOOST_CHECK_EQUAL_COLLECTIONS(expected.begin(), expected.end(), result.begin(), result.end());

  float mean, median, sigma;
  std::tie(mean, median, sigma) = histo.getStats();

  BOOST_CHECK_CLOSE(134.037, sigma, 1e-2);
  BOOST_CHECK_CLOSE(154.733, mean, 1e-2);
  BOOST_CHECK_CLOSE(111.464, median, 1e-2);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(compat4Test) {
  std::vector<float> values{
    9.7458380e-01,  1.6089185e+00,  2.1246729e+00,  2.0380810e+01,  1.2126788e+02,  1.8637155e+02,  9.2191461e+02,  8.2063385e+02,  2.5136021e+02,  6.9224292e-01,
    9.4556081e-01,  3.8256418e-02, -1.1680878e+00,  2.8432534e+00,  2.3142546e+01,  1.1719874e+02,  1.3932178e+02,  8.9128271e+02,  8.5308124e+02,  2.8023126e+02,
    5.5220968e-01,  6.0490292e-01, -8.3996785e-01,  1.2442479e-01,  1.7127032e+00,  2.8168287e+01,  1.0903653e+02,  9.9017204e+01,  8.5283356e+02,  8.8399048e+02,
    -5.2195889e-01, -4.3262571e-01, -3.7933666e-01,  1.0822405e+00,  1.3144805e+00, -1.1634334e-01,  2.4919230e+01,  1.0043754e+02,  5.7376549e+01,  8.2509283e+02,
    -1.7922065e+00, -1.7835485e+00, -4.5839143e-01,  1.0601188e+00,  1.5161804e+00, -4.9567461e-01,  1.3158561e+00,  2.5905865e+01,  9.0337029e+01,  2.1667395e+01,
    -6.9695556e-01,  8.8677365e-01,  5.6401527e-01, -2.4125357e-01, -8.8155591e-01, -1.4742051e+00,  2.7503183e-01,  1.3826104e-01,  2.1163597e+01,  3.8069733e+01,
    -8.5434651e-01,  7.7336085e-01, -1.0087991e+00, -4.9268517e-01,  1.2424717e+00, -1.3651557e-01,  4.6048573e-01,  7.2116679e-01, -8.7556943e-02,  1.0233696e+01,
    -1.8273884e+00, -5.9870118e-01, -2.0351241e+00, -1.4497358e-01, -1.4700560e-01,  8.4688735e-01,  1.7764990e-01, -8.0808467e-01, -1.7884737e+00,  3.7493780e-01,
    -1.5151615e+00,  3.7774354e-02,  2.6230329e-01, -8.0458295e-01, -1.2267905e-02,  1.0753629e+00,  2.4578522e-01, -1.3948788e-01, -1.8167330e-01,  1.3222872e+00,
    3.3589247e-01, -1.1723626e+00, -9.6810353e-01, -1.1642293e+00,  7.7729881e-01, -1.2502791e+00,  4.0728170e-01, -1.8106508e+00, -1.9929892e-01, -4.0931967e-01
  };

  std::vector<float> expected{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 72, 1, 1, 0, 4, 3, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 2, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0
  };

  Euclid::Histogram::Histogram<double> histo(values.begin(), values.end(), KappaSigmaBinning<double>{});
  auto result = histo.getCounts();
  BOOST_CHECK_EQUAL_COLLECTIONS(expected.begin(), expected.end(), result.begin(), result.end());

  float mean, median, sigma;
  std::tie(mean, median, sigma) = histo.getStats();

  BOOST_CHECK_CLOSE(42.9942, sigma, 1e-2);
  BOOST_CHECK_CLOSE(16.1221, mean, 1e-2);
  BOOST_CHECK_CLOSE(0.623711, median, 1e-2);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()

//-----------------------------------------------------------------------------

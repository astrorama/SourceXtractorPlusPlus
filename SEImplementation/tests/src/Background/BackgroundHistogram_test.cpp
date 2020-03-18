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
#include "SEImplementation/Background/SE2/BackgroundHistogram.h"
#include "SEImplementation/Background/SE/HistogramImage.h"
#include "SEUtils/TestUtils.h"

using namespace SourceXtractor;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (BackgroundHistogram_test)

/**
 * Cell from an actual image with a lot of saturated pixels
 * The values of mean and sigma used to initialize the BackgroundHistogram class have been obtained
 * running BackgroundCell::getStats
 *
 * HistogramImage does not need these values, as it computes them itself
 */
struct Histogram_Cell_1 {
  double mean = 31329.7;
  double sigma = 27928.3;
  std::vector<PIXTYPE> values{
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
};

/**
 * Cell from an actual image that ends up with a sigma of 0
 */
struct Histogram_Cell_2 {
  double mean = 2479.76;
  double sigma = 7755.37;
  size_t ndata = 87;
  // A uniform distribution between 0 and 100, plus a Gaussian centered on 2,2
  // with a size of 3 pixels, mean of 500 and sigma of 10
  std::vector<PIXTYPE> values{
    1059.6154, 1087.7279, 1103.0453, 1094.2203, 1183.6117, 1128.3158, 45756.457, 60000., 60000., 60000.,
    971.41034, 1014.7733, 1000.2077, 1094.3527, 1046.2103, 1067.4185, 1080.4036, 60000., 60000., 60000.,
    918.8282, 940.27734, 956.0524, 1001.61487, 970.8749, 1018.0088, 1030.165, 60000., 60000., 60000.,
    888.2462, 839.62164, 882.79474, 939.5607, 940.98663, 990.51996, 965.10614, 60000., 60000., 60000.,
    920.25977, 866.54364, 930.8351, 820.82526, 937.5687, 934.2265, 869.8161, 13792.492, 60000., 41070.016,
    851.4952, 818.1961, 885.6494, 820.99194, 905.52527, 868.70154, 892.26654, 812.6996, 41605.42, 861.28046,
    814.0003, 772.97644, 855.06384, 830.42053, 807.6739, 821.6612, 815.10175, 859.0078, 828.5564, 932.1645,
    818.64264, 789.8802, 763.0362, 844.0108, 813.6501, 835.03705, 778.8387, 804.04535, 809.1223, 820.9612,
    778.79803, 803.10156, 802.82806, 798.21234, 826.3293, 737.83, 840.0581, 774.85315, 815.2481, 798.0622,
    801.5899, 772.59216, 762.28705, 785.5724, 760.2003, 810.8674, 828.8401, 828.8144, 793.1957, 770.833};
};

/**
 * Cell from an actual image
 */
struct Histogram_Cell_3 {
  double mean = 6682.03;
  double sigma = 9749.24;
  size_t ndata = 86;
  // A uniform distribution between 0 and 100, plus a Gaussian centered on 2,2
  // with a size of 3 pixels, mean of 500 and sigma of 10
  std::vector<PIXTYPE> values{
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
};

/**
 * Cell from an actual image that converges into a sigma value of 0
 */
struct Histogram_Cell_4 {
  double mean = 18.969934;
  double sigma = 50.762934;
  size_t ndata = 93;
  std::vector<PIXTYPE> values{
    9.7458380e-01,  1.6089185e+00,  2.1246729e+00,  2.0380810e+01,  1.2126788e+02,  1.8637155e+02,  9.2191461e+02,  8.2063385e+02,  2.5136021e+02,  6.9224292e-01,
    9.4556081e-01,  3.8256418e-02, -1.1680878e+00,  2.8432534e+00,  2.3142546e+01,  1.1719874e+02,  1.3932178e+02,  8.9128271e+02,  8.5308124e+02,  2.8023126e+02,
    5.5220968e-01,  6.0490292e-01, -8.3996785e-01,  1.2442479e-01,  1.7127032e+00,  2.8168287e+01,  1.0903653e+02,  9.9017204e+01,  8.5283356e+02,  8.8399048e+02,
    -5.2195889e-01, -4.3262571e-01, -3.7933666e-01,  1.0822405e+00,  1.3144805e+00, -1.1634334e-01,  2.4919230e+01,  1.0043754e+02,  5.7376549e+01,  8.2509283e+02,
    -1.7922065e+00, -1.7835485e+00, -4.5839143e-01,  1.0601188e+00,  1.5161804e+00, -4.9567461e-01,  1.3158561e+00,  2.5905865e+01,  9.0337029e+01,  2.1667395e+01,
    -6.9695556e-01,  8.8677365e-01,  5.6401527e-01, -2.4125357e-01, -8.8155591e-01, -1.4742051e+00,  2.7503183e-01,  1.3826104e-01,  2.1163597e+01,  3.8069733e+01,
    -8.5434651e-01,  7.7336085e-01, -1.0087991e+00, -4.9268517e-01,  1.2424717e+00, -1.3651557e-01,  4.6048573e-01,  7.2116679e-01, -8.7556943e-02,  1.0233696e+01,
    -1.8273884e+00, -5.9870118e-01, -2.0351241e+00, -1.4497358e-01, -1.4700560e-01,  8.4688735e-01,  1.7764990e-01, -8.0808467e-01, -1.7884737e+00,  3.7493780e-01,
    -1.5151615e+00,  3.7774354e-02,  2.6230329e-01, -8.0458295e-01, -1.2267905e-02,  1.0753629e+00,  2.4578522e-01, -1.3948788e-01, -1.8167330e-01,  1.3222872e+00,
    3.3589247e-01, -1.1723626e+00, -9.6810353e-01, -1.1642293e+00,  7.7729881e-01, -1.2502791e+00,  4.0728170e-01, -1.8106508e+00, -1.9929892e-01, -4.0931967e-01};
};

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(basic_test, Histogram_Cell_1) {
  BackgroundHistogram histogram{mean, sigma, values.size()};
  BOOST_CHECK_EQUAL(mean, histogram.itsMean);
  BOOST_CHECK_EQUAL(sigma, histogram.itsSigma);

  std::for_each(values.begin(), values.end(), [&histogram](PIXTYPE v) { histogram.addDatum(v); });

  PIXTYPE backVal, sigmaVal;
  histogram.getBackGuess(backVal, sigmaVal);
  BOOST_CHECK(checkIsClose(14544.8, backVal));
  BOOST_CHECK(checkIsClose(27666.5, sigmaVal));
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(histogramImage_test, Histogram_Cell_1) {
  HistogramImage<float> histo(VectorImage<float>::create(10, 10, values), nullptr, 10, 10, std::numeric_limits<float>::max());

  auto model = histo.getModeImage();
  auto sigma = histo.getSigmaImage();

  BOOST_CHECK_EQUAL(1, model->getWidth());
  BOOST_CHECK_EQUAL(1, model->getHeight());
  BOOST_CHECK_EQUAL(1, sigma->getWidth());
  BOOST_CHECK_EQUAL(1, sigma->getHeight());

  BOOST_CHECK(checkIsClose(14544.8, model->getValue(0, 0)));
  BOOST_CHECK(checkIsClose(27666.5, sigma->getValue(0, 0)));
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(basic_test2, Histogram_Cell_2) {
  // Note that even though we have 100 pixels, BackgroundCell::getStats will do a first cut that leaves us with 87
  BackgroundHistogram histogram{mean, sigma, ndata};
  BOOST_CHECK_EQUAL(mean, histogram.itsMean);
  BOOST_CHECK_EQUAL(sigma, histogram.itsSigma);

  std::for_each(values.begin(), values.end(), [&histogram](PIXTYPE v) { histogram.addDatum(v); });

  PIXTYPE backVal, sigmaVal;
  histogram.getBackGuess(backVal, sigmaVal);
  BOOST_CHECK(checkIsClose(1142.63, backVal));
  BOOST_CHECK_SMALL(sigmaVal, 1e-5f);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(histogramImage_test2, Histogram_Cell_2) {
  HistogramImage<float> histo(VectorImage<float>::create(10, 10, values), nullptr, 10, 10, std::numeric_limits<float>::max());

  auto model = histo.getModeImage();
  auto sigma = histo.getSigmaImage();

  BOOST_CHECK_EQUAL(1, model->getWidth());
  BOOST_CHECK_EQUAL(1, model->getHeight());
  BOOST_CHECK_EQUAL(1, sigma->getWidth());
  BOOST_CHECK_EQUAL(1, sigma->getHeight());

  BOOST_CHECK(checkIsClose(1142.63, model->getValue(0, 0)));
  BOOST_CHECK_SMALL(sigma->getValue(0, 0), 1e-5f);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(basic_test3, Histogram_Cell_3) {
  BackgroundHistogram histogram{mean, sigma, ndata};
  BOOST_CHECK_EQUAL(mean, histogram.itsMean);
  BOOST_CHECK_EQUAL(sigma, histogram.itsSigma);

  std::for_each(values.begin(), values.end(), [&histogram](PIXTYPE v) { histogram.addDatum(v); });

  PIXTYPE backVal, sigmaVal;
  histogram.getBackGuess(backVal, sigmaVal);
  BOOST_CHECK(checkIsClose(2374.23, backVal));
  BOOST_CHECK(checkIsClose(2069.18, sigmaVal));
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(histogramImage_test3, Histogram_Cell_3) {
  HistogramImage<float> histo(VectorImage<float>::create(10, 10, values), nullptr, 10, 10, std::numeric_limits<float>::max());

  auto model = histo.getModeImage();
  auto sigma = histo.getSigmaImage();

  BOOST_CHECK_EQUAL(1, model->getWidth());
  BOOST_CHECK_EQUAL(1, model->getHeight());
  BOOST_CHECK_EQUAL(1, sigma->getWidth());
  BOOST_CHECK_EQUAL(1, sigma->getHeight());

  BOOST_CHECK(checkIsClose(2374.23, model->getValue(0, 0)));
  BOOST_CHECK(checkIsClose(2069.18, sigma->getValue(0, 0)));
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(basic_test4, Histogram_Cell_4) {
  BackgroundHistogram histogram{mean, sigma, ndata};
  BOOST_CHECK_EQUAL(mean, histogram.itsMean);
  BOOST_CHECK_EQUAL(sigma, histogram.itsSigma);

  std::for_each(values.begin(), values.end(), [&histogram](PIXTYPE v) { histogram.addDatum(v); });

  PIXTYPE backVal, sigmaVal;
  histogram.getBackGuess(backVal, sigmaVal);
  BOOST_CHECK(checkIsClose(-0.134392, backVal));
  BOOST_CHECK_SMALL(sigmaVal, 1e-5f);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(histogramImage_test4, Histogram_Cell_4) {
  HistogramImage<float> histo(VectorImage<float>::create(10, 10, values), nullptr, 10, 10, std::numeric_limits<float>::max());

  auto model = histo.getModeImage();
  auto sigma = histo.getSigmaImage();

  BOOST_CHECK_EQUAL(1, model->getWidth());
  BOOST_CHECK_EQUAL(1, model->getHeight());
  BOOST_CHECK_EQUAL(1, sigma->getWidth());
  BOOST_CHECK_EQUAL(1, sigma->getHeight());

  BOOST_CHECK(checkIsClose(-0.134392, model->getValue(0, 0), 1e-4));
  BOOST_CHECK_SMALL(sigma->getValue(0, 0), 1e-5f);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()

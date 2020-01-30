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
 * @file FFT_test.cpp
 * @date 11/09/18
 * @author Alejandro Alvarez Ayllon
 */

#include <boost/test/unit_test.hpp>
#include <numeric>
#include "SEFramework/FFT/FFT.h"

using namespace SourceXtractor;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (FFT_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE (FFT_float_symmetric_float_test) {
  std::vector<float> scratch(16);
  std::vector<std::complex<float>> complex_data(16);

  auto fwd_plan = FFT<float>::createForwardPlan(1, 4, 4, scratch, complex_data);
  auto inv_plan = FFT<float>::createInversePlan(1, 4, 4, complex_data, scratch);

  std::vector<float> in{
    0.25, 0.00, 0.00, 0.00,
    0.00, 0.25, 0.00, 0.00,
    0.00, 0.00, 0.25, 0.00,
    0.00, 0.00, 0.00, 0.25,
  };
  std::vector<float> out(16);

  FFT<float>::executeForward(fwd_plan, in, complex_data);
  FFT<float>::executeInverse(inv_plan, complex_data, out);
  // Need to normalize!
  float sum = std::accumulate(out.begin(), out.end(), 0.);
  std::for_each(out.begin(), out.end(), [sum](float &v) { v /= sum; });

  BOOST_CHECK_EQUAL_COLLECTIONS(in.begin(), in.end(), out.begin(), out.end());
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE (FFT_float_symmetric_double_test) {
  std::vector<double> scratch(16);
  std::vector<std::complex<double>> complex_data(16);

  auto fwd_plan = FFT<double>::createForwardPlan(1, 4, 4, scratch, complex_data);
  auto inv_plan = FFT<double>::createInversePlan(1, 4, 4, complex_data, scratch);

  std::vector<double> in{
    0.25, 0.00, 0.00, 0.00,
    0.00, 0.25, 0.00, 0.00,
    0.00, 0.00, 0.25, 0.00,
    0.00, 0.00, 0.00, 0.25,
  };
  std::vector<double> out(16);

  FFT<double>::executeForward(fwd_plan, in, complex_data);
  FFT<double>::executeInverse(inv_plan, complex_data, out);
  // Need to normalize!
  double sum = std::accumulate(out.begin(), out.end(), 0.);
  std::for_each(out.begin(), out.end(), [sum](double &v) { v /= sum; });

  BOOST_CHECK_EQUAL_COLLECTIONS(in.begin(), in.end(), out.begin(), out.end());
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()


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

#include "SEFramework/FFT/FFT.h"
#include "SEFramework/FFT/FFTHelper.h"
#include "SEFramework/Image/VectorImage.h"
#include "SEUtils/TestUtils.h"
#include <boost/test/unit_test.hpp>
#include <numeric>

using namespace SourceXtractor;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(FFT_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(FFT_float_symmetric_float_test) {
  std::vector<float> scratch;
  auto               fwd_plan = FFT<float>::createForwardPlan(4, 4, scratch);
  auto               inv_plan = FFT<float>::createInversePlan(4, 4, scratch);

  auto original = VectorImage<float>::create(4, 4,
                                             std::vector<float>{
                                                 0.25, 0.00, 0.00, 0.00,  //
                                                 0.00, 0.25, 0.00, 0.00,  //
                                                 0.00, 0.00, 0.25, 0.00,  //
                                                 0.00, 0.00, 0.00, 0.25,  //
                                             });
  copyImageToFFTWorkArea(*original, scratch);

  // In place!
  FFT<float>::executeForward(fwd_plan, scratch);
  FFT<float>::executeInverse(inv_plan, scratch);

  auto recovered = VectorImage<float>::create(4, 4);
  copyFFTWorkAreaToImage(scratch, *recovered);

  BOOST_CHECK(compareImages(original, recovered));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(FFT_float_symmetric_double_test) {
  std::vector<double> scratch;
  auto               fwd_plan = FFT<double>::createForwardPlan(4, 4, scratch);
  auto               inv_plan = FFT<double>::createInversePlan(4, 4, scratch);

  auto original = VectorImage<double>::create(4, 4,
                                             std::vector<double>{
                                                 0.25, 0.00, 0.00, 0.00,  //
                                                 0.00, 0.25, 0.00, 0.00,  //
                                                 0.00, 0.00, 0.25, 0.00,  //
                                                 0.00, 0.00, 0.00, 0.25,  //
                                             });
  copyImageToFFTWorkArea(*original, scratch);

  // In place!
  FFT<double>::executeForward(fwd_plan, scratch);
  FFT<double>::executeInverse(inv_plan, scratch);

  auto recovered = VectorImage<double>::create(4, 4);
  copyFFTWorkAreaToImage(scratch, *recovered);

  BOOST_CHECK(compareImages(original, recovered));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()

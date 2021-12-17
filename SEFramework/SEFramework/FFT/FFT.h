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
/*
 * @file SEFramework/FFT/FFT.h
 * @date 11/09/18
 * @author Alejandro Alvarez Ayllon
 */

#ifndef _SEFRAMEWORK_FFT_FFT_H
#define _SEFRAMEWORK_FFT_FFT_H

#include <complex>
#include <fftw3.h>
#include <memory>
#include <vector>

namespace SourceXtractor {

/**
 * @brief Wrap FFTW types and functions depending on the primitive type (float or double)
 */
template <typename T>
struct FFTTraits {};

/**
 * @brief Traits for float
 */
template <>
struct FFTTraits<float> {
  typedef fftwf_plan_s                    plan_t;
  typedef fftwf_complex                   complex_t;
  typedef decltype(fftwf_plan_dft_r2c_2d) func_plan_fwd_t;
  typedef decltype(fftwf_plan_dft_c2r_2d) func_plan_inv_t;
  typedef decltype(fftwf_destroy_plan)    func_destroy_plan_t;
  typedef decltype(fftwf_execute_dft_r2c) func_execute_fwd_t;
  typedef decltype(fftwf_execute_dft_c2r) func_execute_inv_t;

  static func_plan_fwd_t*     func_plan_fwd;
  static func_plan_inv_t*     func_plan_inv;
  static func_destroy_plan_t* func_destroy_plan;
  static func_execute_fwd_t*  func_execute_fwd;
  static func_execute_inv_t*  func_execute_inv;
};

/**
 * @brief Traits for double
 */
template <>
struct FFTTraits<double> {
  typedef fftw_plan_s                    plan_t;
  typedef fftw_complex                   complex_t;
  typedef decltype(fftw_plan_dft_r2c_2d) func_plan_fwd_t;
  typedef decltype(fftw_plan_dft_c2r_2d) func_plan_inv_t;
  typedef decltype(fftw_destroy_plan)    func_destroy_plan_t;
  typedef decltype(fftw_execute_dft_r2c) func_execute_fwd_t;
  typedef decltype(fftw_execute_dft_c2r) func_execute_inv_t;

  static func_plan_fwd_t*     func_plan_fwd;
  static func_plan_inv_t*     func_plan_inv;
  static func_destroy_plan_t* func_destroy_plan;
  static func_execute_fwd_t*  func_execute_fwd;
  static func_execute_inv_t*  func_execute_inv;
};

/**
 * @class FFT
 * @brief Wraps the FFTW API with a more C++ like one.
 * @details It builds on top of the FFTTraits as defined previously
 * @tparam T A floating point type
 */
template <typename T>
struct FFT {
  static_assert(std::is_floating_point<T>::value, "FFTTraits only supported for floating point types");

  typedef FFTTraits<T> fftw_traits;

  typedef std::shared_ptr<typename fftw_traits::plan_t> plan_ptr_t;
  typedef typename fftw_traits::complex_t               complex_t;

  /**
   * Create, or reuses if already exists, a 2D FFTW forward plan.
   * @param width
   *    The width of the 2D original data
   * @param height
   *    The height of the 2D original data
   * @param inout
   *    A buffer *in row major order*, with 2*(width/2 + 1) * height positions. This buffer is passed to FFTW
   *    to perform its planning, and *will be overwritten*.
   *    If the memory area is not big enough, createForwardPlan will resize the vector.
   * @return
   *    A pointer to a plan fit to the given dimensions. It can be safely reused between threads.
   */
  static plan_ptr_t createForwardPlan(int width, int height, std::vector<T>& inout);

  /**
   * Create, or reuses if already exists, a 2D FFTW inverse plan.
   * @param width
   *    The width of the 2D original data
   * @param height
   *    The height of the 2D original data
   * @param inout
   *    A buffer *in row major order*, with 2*(width/2 + 1) * height positions. This buffer is passed to FFTW
   *    to perform its planning, and *will be overwritten*.
   *    If the memory area is not big enough, createInversePlan will resize the vector.
   * @return
   *    A pointer to a plan fit to the given dimensions. It can be safely reused between threads.
   */
  static plan_ptr_t createInversePlan(int width, int height, std::vector<T>& inout);

  /**
   * Execute a forward Fourier Transform
   * @param plan
   *    A plan as created by createForwardPlan
   * @param inout
   *    A buffer *in row major order* with the input data. It will be overwritten.
   */
  static void executeForward(plan_ptr_t& plan, std::vector<T>& inout);

  /**
   * Execute an inverse Fourier Transform
   * @param plan
   *    A plan as created by createInversePlan
   * @param inout
   *    A buffer *in row major order* with the input data. It will be overwritten.
   */
  static void executeInverse(plan_ptr_t& plan, std::vector<T>& inout);
};

/**
 * FFTW is best at handling sizes of the form 2^a 3^b 5^c 7^d 11^e 13^f where e+f is either 0 or 1.
 * It can compute any size, but to make things faster, this function will return a new size greater
 * of equal to 'size' that falls into the previously mentioned form. It really improves the performance.
 * Of course, there has to be an equilibrium, if the rounded-up size is too far, then the overhead of
 * computing more data won't pay off. For size >= 1024, if will be rounded up to the next multiple of 512.
 * @param size
 *  The size to round up
 * @return
 *  A value greater of equal that should improve the performance of the transform computation.
 * @see http://www.fftw.org/fftw3_doc/Real_002ddata-DFTs.html
 */
int fftRoundDimension(int size);

extern template struct FFT<float>;
extern template struct FFT<double>;

}  // namespace SourceXtractor

#endif  // _SEFRAMEWORK_FFT_FFT_H

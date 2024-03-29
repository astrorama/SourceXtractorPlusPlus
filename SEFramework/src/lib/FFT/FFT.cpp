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
 * @file FFT.cpp
 * @date 11/09/18
 * @author aalvarez
 */

#include "SEFramework/FFT/FFT.h"
#include <boost/thread/shared_mutex.hpp>
#include <cassert>
#include <fftw3.h>
#include <map>

namespace SourceXtractor {

/**
 * FFTW3 requires a global mutex when creating a plan. Plan executions
 * are, on the other hand, thread safe.
 */
boost::mutex fftw_global_plan_mutex{};

typename FFTTraits<float>::func_plan_fwd_t*     FFTTraits<float>::func_plan_fwd{fftwf_plan_dft_r2c_2d};
typename FFTTraits<float>::func_plan_inv_t*     FFTTraits<float>::func_plan_inv{fftwf_plan_dft_c2r_2d};
typename FFTTraits<float>::func_destroy_plan_t* FFTTraits<float>::func_destroy_plan{fftwf_destroy_plan};
typename FFTTraits<float>::func_execute_fwd_t*  FFTTraits<float>::func_execute_fwd{fftwf_execute_dft_r2c};
typename FFTTraits<float>::func_execute_inv_t*  FFTTraits<float>::func_execute_inv{fftwf_execute_dft_c2r};

typename FFTTraits<double>::func_plan_fwd_t*     FFTTraits<double>::func_plan_fwd{fftw_plan_dft_r2c_2d};
typename FFTTraits<double>::func_plan_inv_t*     FFTTraits<double>::func_plan_inv{fftw_plan_dft_c2r_2d};
typename FFTTraits<double>::func_destroy_plan_t* FFTTraits<double>::func_destroy_plan{fftw_destroy_plan};
typename FFTTraits<double>::func_execute_fwd_t*  FFTTraits<double>::func_execute_fwd{fftw_execute_dft_r2c};
typename FFTTraits<double>::func_execute_inv_t*  FFTTraits<double>::func_execute_inv{fftw_execute_dft_c2r};

int fftRoundDimension(int size) {
  // Precomputed lookup table for the optimal dimension
  static int optimal_dims[] = {
      1,    1,    2,    3,    4,    5,    6,    7,    8,    9,    10,   11,   12,   13,   14,   15,   16,   18,   18,   20,   20,
      21,   22,   24,   24,   25,   26,   27,   28,   30,   30,   32,   32,   33,   35,   35,   36,   39,   39,   39,   40,   42,
      42,   44,   44,   45,   48,   48,   48,   49,   50,   52,   52,   54,   54,   55,   56,   60,   60,   60,   60,   63,   63,
      63,   64,   65,   66,   70,   70,   70,   70,   72,   72,   75,   75,   75,   77,   77,   78,   80,   80,   81,   84,   84,
      84,   88,   88,   88,   88,   90,   90,   91,   96,   96,   96,   96,   96,   98,   98,   99,   100,  104,  104,  104,  104,
      105,  108,  108,  108,  110,  110,  112,  112,  117,  117,  117,  117,  117,  120,  120,  120,  125,  125,  125,  125,  125,
      126,  128,  128,  130,  130,  132,  132,  135,  135,  135,  140,  140,  140,  140,  140,  144,  144,  144,  144,  147,  147,
      147,  150,  150,  150,  154,  154,  154,  154,  156,  156,  160,  160,  160,  160,  162,  162,  165,  165,  165,  168,  168,
      168,  175,  175,  175,  175,  175,  175,  175,  176,  180,  180,  180,  180,  182,  182,  189,  189,  189,  189,  189,  189,
      189,  192,  192,  192,  195,  195,  195,  196,  198,  198,  200,  200,  208,  208,  208,  208,  208,  208,  208,  208,  210,
      210,  216,  216,  216,  216,  216,  216,  220,  220,  220,  220,  224,  224,  224,  224,  225,  231,  231,  231,  231,  231,
      231,  234,  234,  234,  240,  240,  240,  240,  240,  240,  243,  243,  243,  245,  245,  250,  250,  250,  250,  250,  252,
      252,  256,  256,  256,  256,  260,  260,  260,  260,  264,  264,  264,  264,  270,  270,  270,  270,  270,  270,  273,  273,
      273,  275,  275,  280,  280,  280,  280,  280,  288,  288,  288,  288,  288,  288,  288,  288,  294,  294,  294,  294,  294,
      294,  297,  297,  297,  300,  300,  300,  308,  308,  308,  308,  308,  308,  308,  308,  312,  312,  312,  312,  315,  315,
      315,  320,  320,  320,  320,  320,  324,  324,  324,  324,  325,  330,  330,  330,  330,  330,  336,  336,  336,  336,  336,
      336,  343,  343,  343,  343,  343,  343,  343,  350,  350,  350,  350,  350,  350,  350,  351,  352,  360,  360,  360,  360,
      360,  360,  360,  360,  364,  364,  364,  364,  375,  375,  375,  375,  375,  375,  375,  375,  375,  375,  375,  378,  378,
      378,  384,  384,  384,  384,  384,  384,  385,  390,  390,  390,  390,  390,  392,  392,  396,  396,  396,  396,  400,  400,
      400,  400,  405,  405,  405,  405,  405,  416,  416,  416,  416,  416,  416,  416,  416,  416,  416,  416,  420,  420,  420,
      420,  432,  432,  432,  432,  432,  432,  432,  432,  432,  432,  432,  432,  440,  440,  440,  440,  440,  440,  440,  440,
      441,  448,  448,  448,  448,  448,  448,  448,  450,  450,  455,  455,  455,  455,  455,  462,  462,  462,  462,  462,  462,
      462,  468,  468,  468,  468,  468,  468,  480,  480,  480,  480,  480,  480,  480,  480,  480,  480,  480,  480,  486,  486,
      486,  486,  486,  486,  490,  490,  490,  490,  495,  495,  495,  495,  495,  500,  500,  500,  500,  500,  504,  504,  504,
      504,  512,  512,  512,  512,  512,  512,  512,  512,  520,  520,  520,  520,  520,  520,  520,  520,  525,  525,  525,  525,
      525,  528,  528,  528,  539,  539,  539,  539,  539,  539,  539,  539,  539,  539,  539,  540,  546,  546,  546,  546,  546,
      546,  550,  550,  550,  550,  560,  560,  560,  560,  560,  560,  560,  560,  560,  560,  567,  567,  567,  567,  567,  567,
      567,  576,  576,  576,  576,  576,  576,  576,  576,  576,  585,  585,  585,  585,  585,  585,  585,  585,  585,  588,  588,
      588,  594,  594,  594,  594,  594,  594,  600,  600,  600,  600,  600,  600,  616,  616,  616,  616,  616,  616,  616,  616,
      616,  616,  616,  616,  616,  616,  616,  616,  624,  624,  624,  624,  624,  624,  624,  624,  625,  630,  630,  630,  630,
      630,  637,  637,  637,  637,  637,  637,  637,  640,  640,  640,  648,  648,  648,  648,  648,  648,  648,  648,  650,  650,
      660,  660,  660,  660,  660,  660,  660,  660,  660,  660,  672,  672,  672,  672,  672,  672,  672,  672,  672,  672,  672,
      672,  675,  675,  675,  686,  686,  686,  686,  686,  686,  686,  686,  686,  686,  686,  693,  693,  693,  693,  693,  693,
      693,  700,  700,  700,  700,  700,  700,  700,  702,  702,  704,  704,  720,  720,  720,  720,  720,  720,  720,  720,  720,
      720,  720,  720,  720,  720,  720,  720,  728,  728,  728,  728,  728,  728,  728,  728,  729,  735,  735,  735,  735,  735,
      735,  750,  750,  750,  750,  750,  750,  750,  750,  750,  750,  750,  750,  750,  750,  750,  756,  756,  756,  756,  756,
      756,  768,  768,  768,  768,  768,  768,  768,  768,  768,  768,  768,  768,  770,  770,  780,  780,  780,  780,  780,  780,
      780,  780,  780,  780,  784,  784,  784,  784,  792,  792,  792,  792,  792,  792,  792,  792,  800,  800,  800,  800,  800,
      800,  800,  800,  810,  810,  810,  810,  810,  810,  810,  810,  810,  810,  819,  819,  819,  819,  819,  819,  819,  819,
      819,  825,  825,  825,  825,  825,  825,  832,  832,  832,  832,  832,  832,  832,  840,  840,  840,  840,  840,  840,  840,
      840,  864,  864,  864,  864,  864,  864,  864,  864,  864,  864,  864,  864,  864,  864,  864,  864,  864,  864,  864,  864,
      864,  864,  864,  864,  875,  875,  875,  875,  875,  875,  875,  875,  875,  875,  875,  880,  880,  880,  880,  880,  882,
      882,  891,  891,  891,  891,  891,  891,  891,  891,  891,  896,  896,  896,  896,  896,  900,  900,  900,  900,  910,  910,
      910,  910,  910,  910,  910,  910,  910,  910,  924,  924,  924,  924,  924,  924,  924,  924,  924,  924,  924,  924,  924,
      924,  936,  936,  936,  936,  936,  936,  936,  936,  936,  936,  936,  936,  945,  945,  945,  945,  945,  945,  945,  945,
      945,  960,  960,  960,  960,  960,  960,  960,  960,  960,  960,  960,  960,  960,  960,  960,  972,  972,  972,  972,  972,
      972,  972,  972,  972,  972,  972,  972,  975,  975,  975,  980,  980,  980,  980,  980,  990,  990,  990,  990,  990,  990,
      990,  990,  990,  990,  1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1008, 1008, 1008, 1008, 1008, 1008, 1008,
      1008, 1024, 1024, 1024,
  };
  static int n_optimal_dims = sizeof(optimal_dims) / sizeof(int);

  if (size < n_optimal_dims) {
    return optimal_dims[size];
  }
  return (size / 512 + (size % 512 != 0)) * 512;
}

template <typename T>
auto FFT<T>::createForwardPlan(int width, int height, std::vector<T>& inout) -> plan_ptr_t {
  size_t phy_height = height;
  size_t phy_width  = 2 * (width / 2 + 1);
  size_t mem_size = phy_height * phy_width;

  // Make sure the buffers are big enough
  if (inout.size() < mem_size) {
    inout.resize(mem_size);
  }

  // Cache plan, as they can be reused
  static boost::shared_mutex                        mutex;
  static std::map<std::tuple<int, int>, plan_ptr_t> plan_cache;

  boost::upgrade_lock<boost::shared_mutex> read_lock{mutex};

  auto pi = plan_cache.find(std::make_tuple(width, height));
  if (pi != plan_cache.end()) {
    return pi->second;
  }

  // No available plan yet, so get one from FFTW
  boost::upgrade_to_unique_lock<boost::shared_mutex> write_lock{read_lock};
  boost::lock_guard<boost::mutex>                    lock_planner{fftw_global_plan_mutex};

  pi = plan_cache.emplace(
    std::make_tuple(width, height),
    plan_ptr_t{
      fftw_traits::func_plan_fwd(
          height, width, // n0, n1
          inout.data(), reinterpret_cast<complex_t*>(inout.data()), // in, out
          FFTW_ESTIMATE | FFTW_DESTROY_INPUT // flags
        ),
        fftw_traits::func_destroy_plan}
  ).first;

  return pi->second;
}

template <typename T>
auto FFT<T>::createInversePlan(int width, int height, std::vector<T>& inout) -> plan_ptr_t {
  size_t phy_height = height;
  size_t phy_width  = 2 * (width / 2 + 1);
  size_t mem_size = phy_height * phy_width;

  // Make sure the buffers are big enough
  if (inout.size() < mem_size) {
    inout.resize(mem_size);
  }

  // Cache plan, as they can be reused
  static boost::shared_mutex                        mutex;
  static std::map<std::tuple<int, int>, plan_ptr_t> plan_cache;

  boost::upgrade_lock<boost::shared_mutex> read_lock{mutex};

  auto pi = plan_cache.find(std::make_tuple(width, height));
  if (pi != plan_cache.end()) {
    return pi->second;
  }

  // No available plan yet, so get one from FFTW
  boost::upgrade_to_unique_lock<boost::shared_mutex> write_lock{read_lock};
  boost::lock_guard<boost::mutex>                    lock_planner{fftw_global_plan_mutex};

  pi = plan_cache.emplace(
    std::make_tuple(width, height),
    plan_ptr_t{
      fftw_traits::func_plan_inv(
        height, width,       // n0, n1
        reinterpret_cast<complex_t*>(inout.data()), inout.data(),  // in, out
        FFTW_ESTIMATE | FFTW_DESTROY_INPUT // flags
      ),
      fftw_traits::func_destroy_plan}
 ).first;

  return pi->second;
}

template <typename T>
void FFT<T>::executeForward(plan_ptr_t& plan, std::vector<T>& inout) {
  fftw_traits::func_execute_fwd(plan.get(), inout.data(), reinterpret_cast<complex_t*>(inout.data()));
}

template <typename T>
void FFT<T>::executeInverse(plan_ptr_t& plan, std::vector<T>& inout) {
  fftw_traits::func_execute_inv(plan.get(), reinterpret_cast<complex_t*>(inout.data()), inout.data());
}

template struct FFT<float>;
template struct FFT<double>;

}  // namespace SourceXtractor

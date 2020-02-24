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

#ifndef SOURCEXTRACTORPLUSPLUS_KAPPASIGMABINNING_H
#define SOURCEXTRACTORPLUSPLUS_KAPPASIGMABINNING_H

#include "Histogram/Histogram.h"

namespace SourceXtractor {

/**
 * Functor that estimates the number of bins depending on the standard deviation of the data,
 * applying two cuts to remain robuts to outliers.
 *
 * On a first step, data that is outside \f$ \kappa_1 \sigma_{data} \pm \mu_{data} \f$ is filtered out,
 * and the standard deviation and the mean of the remaining data points is re-computed.
 *
 * The final histogram bins will cover the range \f$ \kappa_2 \sigma_{filtered} \pm \mu_{filtered} \f$.
 * Anything outside will be dropped.
 *
 * The number of bins is directly proportional to \f$ \kappa_2 \f$ and the number of data points, and limited
 * to 4096 bins by default.
 */
template <typename VarType>
class KappaSigmaBinning: public Euclid::Histogram::BinStrategy<VarType> {
public:
  /**
   * Constructor
   * @param kappa1
   *    Number of sigmas for the first data cut
   * @param kappa2
   *    Number of sigmas for the histogram limits
   * @param min_pixels
   *    Minimum number of "mode pixels"
   * @param max_size
   *    Maximum number of bins
   */
  KappaSigmaBinning(float kappa1 = 2., float kappa2 = 5., size_t min_pixels = 4, size_t max_size = 4096)
    : m_kappa(kappa1), m_kappa2(kappa2), m_min_pixels(min_pixels), m_max_size(max_size), m_start(0), m_step(1) {}

  /**
   * Get the list of bin edges for the given data points
   * @tparam Iterator
   *    Iterator type as passed to Histogram
   * @param begin
   *    Beginning of the data
   * @param end
   *    End of the data
   * @return
   *    A vector with the bin *edges*
   */
  template<typename Iterator>
  void computeBins(Iterator begin, Iterator end) {
    // Compute mean and standard deviation of the original data set
    float mean, sigma;
    size_t ndata;
    Stats stats;
    for (auto i = begin; i != end; ++i) {
      stats(*i);
    }
    std::tie(mean, sigma, ndata) = stats.get();

    assert(sigma >= 0);

    // Cuts
    auto lcut = mean - (sigma + 0.5) * m_kappa;
    auto hcut = mean + (sigma + 0.5) * m_kappa;

    // Re-compute mean and standard deviation of values within cut
    stats.reset();
    for (auto i = begin; i != end; ++i) {
      if (*i >= lcut && *i <= hcut)
        stats(*i);
    }
    std::tie(mean, sigma, ndata) = stats.get();

    assert(ndata > 0);

    // Number of bins
    m_nbins = computeBinCount(ndata);

    // Bin size and offset
    m_step = 2 * (m_kappa2 * sigma) / m_nbins;
    if (m_step == 0)
      m_step = 1;
    auto bin_zero = mean - (m_kappa2 * sigma);
    auto bin_const = 0.49999 - bin_zero / m_step;
    m_start = -bin_const * m_step;
  }

  ssize_t getBinIndex(VarType value) const final {
    return (value - m_start) / m_step;
  }

  VarType getEdge(size_t e) const final {
    return e * m_step + m_start;
  }

  VarType getBin(size_t i) const final {
    return (i + 0.5) * m_step + m_start;
  }

private:
  float m_kappa, m_kappa2;
  size_t m_min_pixels, m_max_size;
  using Euclid::Histogram::BinStrategy<VarType>::m_nbins;
  VarType m_start, m_step;

  size_t computeBinCount(size_t ndata) const {
    size_t nbins = ndata * std::sqrt(M_2_PI) * m_kappa2 / m_min_pixels + 1;
    return std::min(nbins, static_cast<size_t>(4096));
  }

  /**
   * Online mean and standard deviation computation
   * Similar to boost::accumulators, but this turned out to be faster
   */
  struct Stats {
    VarType mean = 0, sigma = 0;
    size_t ndata = 0;

    void operator() (VarType v) {
      mean += v;
      sigma += v * v;
      ++ndata;
    }

    std::tuple<VarType, VarType, size_t> get() {
      mean /= ndata;
      sigma = sigma / ndata - mean * mean;
      if (sigma > 0.)
        sigma = std::sqrt(sigma);
      else
        sigma = 0.;
      return std::make_tuple(mean, sigma, ndata);
    }

    void reset() {
      mean = sigma = 0;
      ndata = 0;
    }
  };
};

} // end of namespace SourceXtractor

#endif //SOURCEXTRACTORPLUSPLUS_KAPPASIGMABINNING_H

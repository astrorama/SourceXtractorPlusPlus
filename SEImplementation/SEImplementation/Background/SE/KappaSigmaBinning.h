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

#include <iostream>
#include <iterator>

#include "ElementsKernel/Exception.h"
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
    : m_kappa(kappa1), m_kappa2(kappa2), m_min_pixels(min_pixels), m_max_size(max_size),
      m_scale(1), m_zero(0), m_const(0) {}

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
    // *begin is dereferenced below, and there are no bins to compute anyway
    if (begin == end) {
      throw Elements::Exception() << "Can't compute background histogram bins: no data points";
    }

    // Compute mean and standard deviation of the original data set
    double mean, sigma;
    size_t ndata;
    Stats stats;
    for (auto i = begin; i != end; ++i) {
      stats(*i);
    }
    std::tie(mean, sigma, ndata) = stats.get();

    if (sigma == 0) {
      sigma = std::abs(*begin - mean);
    }
    else {
      sigma *= m_kappa;
    }

    // Cuts
    auto lcut = mean - sigma;
    auto hcut = mean + sigma;

    // Re-compute mean and standard deviation of values within cut
    stats.reset();
    for (auto i = begin; i != end; ++i) {
      if (*i >= lcut && *i <= hcut)
        stats(*i);
    }
    std::tie(mean, sigma, ndata) = stats.get();

    // This case shouldn't be reachable anymore with fixes in #649, but still
    // throw here just in case (previously was a bare assert); this way at least
    // it gives a more informative error message.
    if (ndata == 0) {
      throw Elements::Exception()
          << "Can't compute background histogram bins: none of the " << std::distance(begin, end)
          << " input values lie within " << m_kappa << " sigma of their mean (" << mean
          << "), which should not be possible for finite data";
    }

    // Number of bins
    m_nbins = computeBinCount(ndata);

    // Bin size and offset
    m_scale = 2 * (m_kappa2 * sigma) / m_nbins;
    if (m_scale == 0)
      m_scale = 1;
    m_zero = mean - (m_kappa2 * sigma);
    m_const = 0.49999 - m_zero / m_scale;
  }

  ssize_t getBinIndex(VarType value) const final {
    return value / m_scale + m_const;
  }

  VarType getEdge(size_t e) const final {
    return (static_cast<float>(e) - 0.5) * m_scale + m_zero;
  }

  VarType getBin(size_t i) const final {
    return i * m_scale + m_zero;
  }

private:
  VarType m_kappa, m_kappa2;
  size_t m_min_pixels, m_max_size;
  using Euclid::Histogram::BinStrategy<VarType>::m_nbins;
  VarType m_scale, m_zero, m_const;

  size_t computeBinCount(size_t ndata) const {
    size_t nbins = ndata * std::sqrt(M_2_PI) * m_kappa2 / m_min_pixels + 1;
    return std::min(nbins, static_cast<size_t>(4096));
  }

  /**
   * Online mean and standard deviation computation
   */
  struct Stats {
    /// Everything is accumulated as an offset from the first value seen, so that the
    /// sums stay on the scale of the spread rather than that of the data
    double shift = 0, sum = 0, sum_sq = 0;
    size_t ndata = 0;

    // Shifting by a value from the data largely avoids the catastrophic cancellation
    // that E[X^2] - E[X]^2 suffers when the spread is small next to the magnitude.
    // See https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance, under
    // "Computing shifted data"; Welford's algorithm there fixes it too, but costs
    // about 4x as much, as it performs a division per sample.
    void operator() (VarType v) {
      if (ndata == 0) {
        shift = v;
      }

      double offset = v - shift;
      sum += offset;
      sum_sq += offset * offset;
      ++ndata;
    }

    std::tuple<double, double, size_t> get() {
      if (ndata == 0) {
        return std::make_tuple(0., 0., ndata);
      }

      double mean = shift + sum / ndata;
      double sigma = (sum_sq - sum * sum / ndata) / ndata;

      if (sigma > std::numeric_limits<double>::epsilon())
        sigma = std::sqrt(sigma);
      else
        sigma = 0.;

      return std::make_tuple(mean, sigma, ndata);
    }

    void reset() {
      shift = sum = sum_sq = 0;
      ndata = 0;
    }
  };
};

} // end of namespace SourceXtractor

#endif //SOURCEXTRACTORPLUSPLUS_KAPPASIGMABINNING_H

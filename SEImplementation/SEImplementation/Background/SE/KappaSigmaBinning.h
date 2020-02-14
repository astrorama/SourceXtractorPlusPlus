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

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/count.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/variance.hpp>

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
class KappaSigmaBinning {
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
    : m_kappa(kappa1), m_kappa2(kappa2), m_min_pixels(min_pixels), m_max_size(max_size) {}

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
  auto operator()(Iterator begin, Iterator end) const -> std::vector<typename std::iterator_traits<Iterator>::value_type> {
    using value_type = typename std::iterator_traits<Iterator>::value_type;
    namespace ba = boost::accumulators;

    // Compute mean and standard deviation of the original data set
    ba::accumulator_set<value_type, ba::stats<ba::tag::variance, ba::tag::mean>> data_acc;
    std::for_each(begin, end, std::bind<void>(std::ref(data_acc), std::placeholders::_1));
    auto sigma = std::sqrt(ba::variance(data_acc));
    auto mean = ba::mean(data_acc);

    // Cuts
    auto lcut = mean - sigma * m_kappa;
    auto hcut = mean + sigma * m_kappa;

    // Re-compute mean and standard deviation of values within cut
    ba::accumulator_set<value_type, ba::stats<ba::tag::variance, ba::tag::mean, ba::tag::count>> cut_acc;
    std::for_each(begin, end, [lcut, hcut, &cut_acc](value_type v){
      if (v >= lcut && v <= hcut)
        cut_acc(v);
    });

    sigma = std::sqrt(ba::variance(cut_acc));
    mean = ba::mean(cut_acc);
    size_t ndata = ba::count(cut_acc);

    // Number of bins
    size_t nbins = computeBinCount(ndata);

    // Bin size and offset
    auto bin_scale = 2 * (m_kappa2 * sigma) / nbins;
    if (bin_scale == 0)
      bin_scale = 1;
    auto bin_zero = mean - (m_kappa2 * sigma);
    auto bin_const = 0.49999 - bin_zero / bin_scale;

    // Initialize edges
    auto v = -bin_const * bin_scale;
    size_t nedges = nbins + 1;
    std::vector<value_type> result(nedges);
    std::generate(result.begin(), result.end(), [bin_scale, &v]() {
      auto aux = v;
      v += bin_scale;
      return aux;
    });
    return result;
  }

private:
  float m_kappa, m_kappa2;
  size_t m_min_pixels, m_max_size;

  size_t computeBinCount(size_t ndata) const {
    size_t nbins = ndata * std::sqrt(M_2_PI) * m_kappa2 / m_min_pixels + 1;
    return std::min(nbins, static_cast<size_t>(4096));
  }
};

} // end of namespace SourceXtractor

#endif //SOURCEXTRACTORPLUSPLUS_KAPPASIGMABINNING_H

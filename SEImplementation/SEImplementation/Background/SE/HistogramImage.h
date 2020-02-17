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

#ifndef SOURCEXTRACTORPLUSPLUS_HISTOGRAMIMAGE_H
#define SOURCEXTRACTORPLUSPLUS_HISTOGRAMIMAGE_H

#include "SEFramework/Image/ImageBase.h"
#include "SEFramework/Image/VectorImage.h"

namespace SourceXtractor {

template<typename T>
class HistogramImage {
public:

  HistogramImage(const std::shared_ptr<Image<T>>& image, const std::shared_ptr<Image<T>>& variance, T var_threshold,
                 int cell_w, int cell_h,
                 T invalid_value, T kappa1=2, T kappa2=5, T kappa3=3,
                 T rtol = 1e-4, size_t max_iter = 100);

  virtual ~HistogramImage() = default;

  std::shared_ptr<Image<T>> getModeImage() const;

  std::shared_ptr<Image<T>> getVarianceImage() const;

  std::shared_ptr<Image<T>> getWeightImage() const;

  std::shared_ptr<Image<T>> getWeightVarianceImage() const;

  T getMedianVariance() const;

private:
  std::shared_ptr<const Image<T>> m_image, m_variance;
  std::shared_ptr<VectorImage<T>> m_mode, m_sigma, m_weight_mode, m_weight_sigma;
  T m_variance_threshold;
  int m_cell_w, m_cell_h;
  T m_invalid, m_kappa1, m_kappa2, m_kappa3, m_rtol;
  size_t m_max_iter;

  std::tuple<T, T> getBackGuess(const std::vector<T> &data) const;
  void processCell(int x, int y);
};

extern template
class HistogramImage<SeFloat>;

} // end of namespace SourceXtractor

#endif // SOURCEXTRACTORPLUSPLUS_HISTOGRAMIMAGE_H

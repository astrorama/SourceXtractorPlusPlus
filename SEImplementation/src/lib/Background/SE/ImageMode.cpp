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

#include <Histogram/Histogram.h> // From Alexandria

#include "SEFramework/Image/VectorImage.h"
#include "SEImplementation/Background/SE/ImageMode.h"
#include "SEImplementation/Background/SE/KappaSigmaBinning.h"


using Euclid::Histogram::Histogram;

namespace SourceXtractor {

template<typename T>
ImageMode<T>::ImageMode(const std::shared_ptr<Image<T>>& image, const std::shared_ptr<Image<T>>& variance,
                        int cell_w, int cell_h,
                        T invalid_value, T kappa1, T kappa2, T kappa3,
                        T rtol, size_t max_iter): m_image(image),
                                                            m_cell_w(cell_w), m_cell_h(cell_h),
                                                            m_invalid(invalid_value),
                                                            m_kappa1(kappa1), m_kappa2(kappa2), m_kappa3(kappa3),
                                                            m_rtol(rtol), m_max_iter(max_iter) {
  auto hist_width = std::div(image->getWidth(), m_cell_w);
  if (hist_width.rem)
    ++hist_width.quot;
  auto hist_height = std::div(image->getHeight(), m_cell_h);
  if (hist_height.rem)
    ++hist_height.quot;

  m_mode = VectorImage<T>::create(hist_width.quot, hist_height.quot);
  m_sigma = VectorImage<T>::create(hist_width.quot, hist_height.quot);

  if (variance) {
    m_var_mode = VectorImage<T>::create(hist_width.quot, hist_height.quot);
    m_var_sigma = VectorImage<T>::create(hist_width.quot, hist_height.quot);

    for (int y = 0; y < m_mode->getHeight(); ++y) {
      for (int x = 0; x < m_mode->getWidth(); ++x) {
        processCell(*image, x, y, *m_mode, *m_sigma);
        processCell(*variance, x, y, *m_var_mode, *m_var_sigma);
      }
    }
  }
  else {
    for (int y = 0; y < m_mode->getHeight(); ++y) {
      for (int x = 0; x < m_mode->getWidth(); ++x) {
        processCell(*image, x, y, *m_mode, *m_sigma);
      }
    }
  }
}

template<typename T>
std::shared_ptr<VectorImage<T>> ImageMode<T>::getModeImage() const {
  return m_mode;
}

template<typename T>
std::shared_ptr<VectorImage<T>> ImageMode<T>::getSigmaImage() const {
  return m_sigma;
}

template<typename T>
std::shared_ptr<VectorImage<T>> ImageMode<T>::getVarianceModeImage() const {
  return m_var_mode;
}

template<typename T>
std::shared_ptr<VectorImage<T>> ImageMode<T>::getVarianceSigmaImage() const {
  return m_var_sigma;
}

template<typename T>
std::tuple<T, T> ImageMode<T>::getBackGuess(const std::vector<T>& data) const {
  Histogram<double, int64_t> histo(data.begin(), data.end(), KappaSigmaBinning<double>(m_kappa1, m_kappa2));

  auto ref_bin = histo.getBinEdges(0);
  auto atol = (ref_bin.second - ref_bin.first) * 0.1;

  T mean, median, sigma;
  std::tie(mean, median, sigma) = histo.getStats();
  T prev_sigma = sigma * 10;

  assert(!std::isnan(mean));

  for (size_t iter = 0; iter < m_max_iter && sigma > atol && std::abs(sigma / prev_sigma - 1.0) > m_rtol; ++iter) {
    histo.clip(median - sigma * m_kappa3, median + sigma * m_kappa3);
    prev_sigma = sigma;
    std::tie(mean, median, sigma) = histo.getStats();
  }

  // Sigma is 0
  T mode;
  if (std::abs(sigma) == 0) {
    mode = mean;
  }
  // Not crowded: mean and median do not differ more than 30%
  else if (std::abs((mean - median) / sigma) < 0.3) {
    mode = 2.5 * median - 1.5 * mean;
  }
  // Crowded case: we use the median
  else {
    mode = median;
  }
  return std::make_tuple(mode, sigma);
}

template<typename T>
void ImageMode<T>::processCell(const Image<T>& img, int x, int y,
                               VectorImage<T>& out_mode, VectorImage<T>& out_sigma) const {
  int off_x = x * m_cell_w;
  int off_y = y * m_cell_h;
  int w = std::min(m_cell_w, img.getWidth() - off_x);
  int h = std::min(m_cell_h, img.getHeight() - off_y);

  auto img_chunk_ptr = img.getChunk(off_x, off_y, w, h);
  auto& img_chunk = *img_chunk_ptr;

  std::vector<T> filtered;
  filtered.reserve(w * h);

  for (int y = 0; y < h; ++y) {
    for (int x = 0; x < w; ++x) {
      auto v = img_chunk.getValue(x, y);
      if (v != m_invalid)
        filtered.emplace_back(v);
    }
  }

  if (filtered.size() / static_cast<float>(w * h) < 0.5) {
    out_mode.setValue(x, y, m_invalid);
    out_sigma.setValue(x, y, m_invalid);
  }
  else {
    T mode, sigma;
    std::tie(mode, sigma) = getBackGuess(filtered);
    out_mode.setValue(x, y, mode);
    out_sigma.setValue(x, y, sigma);
  }
}

template
class ImageMode<SeFloat>;

} // end of namespace SourceXtractor

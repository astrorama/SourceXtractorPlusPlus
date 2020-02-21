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

#include "SEImplementation/Background/SE/SEBackgroundLevelAnalyzer.h"

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/median.hpp>

#include "SEFramework/Image/ConstantImage.h"
#include "SEFramework/Image/MaskedImage.h"
#include "SEFramework/Image/ProcessedImage.h"
#include "SEImplementation/Background/Utils.h"
#include "SEImplementation/Background/SE/HistogramImage.h"
#include "SEImplementation/Background/SE/MedianFilter.h"
#include "SEImplementation/Background/SE/ReplaceUndefImage.h"

namespace SourceXtractor {

SEBackgroundLevelAnalyzer::SEBackgroundLevelAnalyzer(const std::vector<int>& cell_size,
                                                     const std::vector<int>& smoothing_box,
                                                     const WeightImageConfig::WeightType weight_type)
  : m_weight_type(weight_type) {
  assert(cell_size.size() > 0 && cell_size.size() < 3);
  assert(smoothing_box.size() > 0 && smoothing_box.size() < 3);
  m_cell_size[0] = cell_size.front();
  m_cell_size[1] = cell_size.back();
  m_smoothing_box[0] = smoothing_box.front();
  m_smoothing_box[1] = smoothing_box.back();
}

static float computeScaling(const std::shared_ptr<Image<DetectionImage::PixelType>>& variance,
                            const std::shared_ptr<Image<WeightImage::PixelType>>& weight) {
  namespace ba = boost::accumulators;

  ba::accumulator_set<WeightImage::PixelType, ba::stats<ba::tag::median>> acc;

  for (int y = 0; y < variance->getHeight(); ++y) {
    for (int x = 0; x < variance->getWidth(); ++x) {
      auto w = weight->getValue(x, y);
      auto v = variance->getValue(x, y);
      auto ratio = (v * v) / w;
      if (ratio > 0) {
        acc(ratio);
      }
    }
  }

  return ba::median(acc);
}

BackgroundModel SEBackgroundLevelAnalyzer::analyzeBackground(
  std::shared_ptr<DetectionImage> image, std::shared_ptr<WeightImage> variance_map,
  std::shared_ptr<Image<unsigned char>> mask, WeightImage::PixelType variance_threshold) const {

  const auto mask_value = std::numeric_limits<DetectionImage::PixelType>::min();

  if (mask != nullptr) {
    bck_model_logger.debug() << "\tMask image with size: (" << mask->getWidth() << "," << mask->getHeight() << ")";

    // make sure the dimensions are the same
    if (image->getWidth() != mask->getWidth())
      throw Elements::Exception() << "X-dims do not match: image=" << image->getWidth() << " mask=" << mask->getWidth();
    if (image->getHeight() != mask->getHeight())
      throw Elements::Exception() << "Y-dims do not match: image=" << image->getHeight() << " mask="
                                  << mask->getHeight();

    image = MaskedImage<DetectionImage::PixelType, uint8_t>::create(image, mask, mask_value);
  }

  if (variance_map!=nullptr)
  {
    bck_model_logger.debug() << "\tVariance image with size: (" << variance_map->getWidth() << "," << variance_map->getHeight() << ")";
    // make sure the dimensions are the same
    if (image->getWidth()!=variance_map->getWidth())
      throw Elements::Exception() << "X-dims do not match: image=" << image->getWidth() << " variance=" << variance_map->getWidth();
    if (image->getHeight()!=variance_map->getHeight())
      throw Elements::Exception() << "Y-dims do not match: image=" << image->getHeight() << " variance=" << variance_map->getHeight();

    // Anything above the threshold is masked out
    image = MaskedImage<DetectionImage::PixelType, WeightImage::PixelType, std::greater_equal>::create(
      image, variance_map, mask_value, variance_threshold);
    variance_map = MaskedImage<WeightImage::PixelType, WeightImage::PixelType, std::greater_equal>::create(
      variance_map, variance_map, mask_value, variance_threshold);
  }

  // Create histogram model for the image
  HistogramImage<SeFloat> histo(image, m_cell_size[0], m_cell_size[1], mask_value, 2, 5, 3);
  auto mode = histo.getModeImage();
  auto var = histo.getSigmaImage();

  // Interpolate missing values
  mode = ReplaceUndefImage<SeFloat>::create(mode, mask_value);
  var = ReplaceUndefImage<SeFloat>::create(var, mask_value);

  // Smooth with the smooth_box (median filtering)
  std::tie(mode, var) = MedianFilter<DetectionImage::PixelType>(m_smoothing_box)(*mode, *var);

  SeFloat scaling = 99999;

  if (variance_map) {
    // Create histogram model for the variance image
    HistogramImage<SeFloat> var_histo(variance_map, m_cell_size[0], m_cell_size[1], mask_value, 2, 5, 3);
    auto weight = var_histo.getModeImage();
    auto weight_var = var_histo.getSigmaImage();
    // Smooth with the smooth_box (median filtering)
    std::tie(weight, weight_var) = MedianFilter<WeightImage::PixelType>(m_smoothing_box)(*weight, *weight_var);
    // Compute scaling
    scaling = computeScaling(var, weight);
    // Transform RMS to variance
    var = MultiplyImage<WeightImage::PixelType>::create(var, var);
  }
  else {
    auto sigma = histo.getMedianSigma();
    var = ConstantImage<SeFloat>::create(image->getWidth(), image->getHeight(), sigma*sigma);
  }
  return BackgroundModel(mode, var, scaling);
}

} // end of namespace SourceXtractor

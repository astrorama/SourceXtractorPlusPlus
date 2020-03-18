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

#include "SEFramework/Image/ConstantImage.h"
#include "SEFramework/Image/MaskedImage.h"
#include "SEFramework/Image/ProcessedImage.h"
#include "SEFramework/Image/BufferedImage.h"
#include "SEFramework/Image/ScaledImageSource.h"
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

static float computeScaling(const std::shared_ptr<VectorImage<DetectionImage::PixelType>>& variance,
                            const std::shared_ptr<VectorImage<WeightImage::PixelType>>& weight) {
  std::vector<float> ratios;
  ratios.reserve(variance->getWidth() * variance->getHeight());

  for (int y = 0; y < variance->getHeight(); ++y) {
    for (int x = 0; x < variance->getWidth(); ++x) {
      auto w = weight->getValue(x, y);
      if (w > 0) {
        auto v = variance->getValue(x, y);
        auto ratio = (v * v) / w;
        if (ratio > 0) {
          ratios.emplace_back(ratio);
        }
      }
    }
  }

  std::sort(ratios.begin(), ratios.end());
  if (ratios.size() % 2 == 1) {
    return ratios[ratios.size() / 2];
  }
  return (ratios[ratios.size() / 2] + ratios[ratios.size() / 2 - 1]) / 2;
}

BackgroundModel SEBackgroundLevelAnalyzer::analyzeBackground(
  std::shared_ptr<DetectionImage> image, std::shared_ptr<WeightImage> variance_map,
  std::shared_ptr<Image<unsigned char>> mask, WeightImage::PixelType variance_threshold) const {

  const auto mask_value = std::numeric_limits<DetectionImage::PixelType>::lowest();

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

  if (variance_map != nullptr) {
    bck_model_logger.debug() << "\tVariance image with size: (" << variance_map->getWidth() << ","
                             << variance_map->getHeight() << ")";
    // make sure the dimensions are the same
    if (image->getWidth() != variance_map->getWidth())
      throw Elements::Exception() << "X-dims do not match: image=" << image->getWidth() << " variance="
                                  << variance_map->getWidth();
    if (image->getHeight() != variance_map->getHeight())
      throw Elements::Exception() << "Y-dims do not match: image=" << image->getHeight() << " variance="
                                  << variance_map->getHeight();

    // Anything above the threshold is masked out
    image = MaskedImage<DetectionImage::PixelType, WeightImage::PixelType, std::greater_equal>::create(
      image, variance_map, mask_value, variance_threshold);
    variance_map = MaskedImage<WeightImage::PixelType, WeightImage::PixelType, std::greater_equal>::create(
      variance_map, variance_map, mask_value, variance_threshold);
  }

  // Create histogram model for the image
  HistogramImage<DetectionImage::PixelType> histo(image, variance_map, m_cell_size[0], m_cell_size[1], mask_value, 2, 5, 3);
  auto mode = histo.getModeImage();
  auto var = histo.getSigmaImage();

  // Interpolate missing values
  // The result is "materialized" into a VectorImage to avoid redundant computations on the next steps
  mode = VectorImage<DetectionImage::PixelType>::create(
    ReplaceUndefImage<DetectionImage::PixelType>::create(mode, mask_value));
  var = VectorImage<DetectionImage::PixelType>::create(
    ReplaceUndefImage<DetectionImage::PixelType>::create(var, mask_value));

  // Smooth with the smooth_box (median filtering)
  std::tie(mode, var) = MedianFilter<DetectionImage::PixelType>(m_smoothing_box)(*mode, *var);

  SeFloat scaling = 99999;

  std::shared_ptr<Image<DetectionImage::PixelType>> final_bg, final_var;

  if (variance_map) {
    // Create histogram model for the variance image
    auto weight = histo.getVarianceModeImage();
    auto weight_var = histo.getVarianceSigmaImage();
    // Interpolate missing values
    weight = VectorImage<WeightImage::PixelType>::create(
      ReplaceUndefImage<DetectionImage::PixelType>::create(weight, mask_value));
    // Smooth with the smooth_box (median filtering)
    std::tie(weight, weight_var) = MedianFilter<WeightImage::PixelType>(m_smoothing_box)(*weight, *weight_var);
    // Compute scaling
    scaling = computeScaling(var, weight);
    // Transform RMS to variance
    final_var = MultiplyImage<DetectionImage::PixelType>::create(var, var);
    final_var = BufferedImage<DetectionImage::PixelType>::create(
      std::make_shared<ScaledImageSource<DetectionImage::PixelType>>(
        final_var, image->getWidth(), image->getHeight(),
        ScaledImageSource<DetectionImage::PixelType>::InterpolationType::BICUBIC
      )
    );
  }
  else {
    auto sigma = histo.getMedianSigma();
    final_var = ConstantImage<DetectionImage::PixelType>::create(image->getWidth(), image->getHeight(), sigma * sigma);
  }

  final_bg = BufferedImage<DetectionImage::PixelType>::create(
    std::make_shared<ScaledImageSource<DetectionImage::PixelType>>(
      mode, image->getWidth(), image->getHeight(),
      ScaledImageSource<DetectionImage::PixelType>::InterpolationType::BICUBIC
    )
  );

  return BackgroundModel(final_bg, final_var, scaling);
}

} // end of namespace SourceXtractor

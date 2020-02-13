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

#include "SEImplementation/Background/Utils.h"
#include "SEImplementation/Background/SE/SEBackgroundLevelAnalyzer.h"

#include "SEFramework/Image/MaskedImage.h"

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

BackgroundModel SEBackgroundLevelAnalyzer::analyzeBackground(
  std::shared_ptr<DetectionImage> image, std::shared_ptr<WeightImage> variance_map,
  std::shared_ptr<Image<unsigned char>> mask, WeightImage::PixelType variance_threshold) const {

  if (mask!=nullptr)
  {
    bck_model_logger.debug() << "\tMask image with size: (" << mask->getWidth() << "," << mask->getHeight() << ")";

    // make sure the dimensions are the same
    if (image->getWidth()!=mask->getWidth())
      throw Elements::Exception() << "X-dims do not match: image=" << image->getWidth() << " mask=" << mask->getWidth();
    if (image->getHeight()!=mask->getHeight())
      throw Elements::Exception() << "Y-dims do not match: image=" << image->getHeight() << " mask=" << mask->getHeight();

    image = MaskedImage<DetectionImage::PixelType, unsigned char>::create(image, mask, std::numeric_limits<DetectionImage::PixelType>::min());
  }

  if (variance_map!=nullptr)
  {
    bck_model_logger.debug() << "\tVariance image with size: (" << variance_map->getWidth() << "," << variance_map->getHeight() << ")";
    // make sure the dimensions are the same
    if (image->getWidth()!=variance_map->getWidth())
      throw Elements::Exception() << "X-dims do not match: image=" << image->getWidth() << " variance=" << variance_map->getWidth();
    if (image->getHeight()!=variance_map->getHeight())
      throw Elements::Exception() << "Y-dims do not match: image=" << image->getHeight() << " variance=" << variance_map->getHeight();
  }


  return BackgroundModel(nullptr, nullptr, 1);
}

} // end of namespace SourceXtractor

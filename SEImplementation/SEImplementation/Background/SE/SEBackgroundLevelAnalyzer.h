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

#ifndef SOURCEXTRACTORPLUSPLUS_SEBACKGROUNDLEVELANALYZER_H
#define SOURCEXTRACTORPLUSPLUS_SEBACKGROUNDLEVELANALYZER_H

#include "SEFramework/Frame/Frame.h"
#include "SEFramework/Image/Image.h"
#include "SEFramework/Background/BackgroundAnalyzer.h"
#include "SEImplementation/Configuration/WeightImageConfig.h"

namespace SourceXtractor {

class SEBackgroundLevelAnalyzer : public BackgroundAnalyzer {
public:
  SEBackgroundLevelAnalyzer(const std::vector<int>& cell_size, const std::vector<int>& smoothing_box,
                            const WeightImageConfig::WeightType weight_type);

  virtual ~SEBackgroundLevelAnalyzer() = default;

  BackgroundModel analyzeBackground(
    std::shared_ptr<DetectionImage> image, std::shared_ptr<WeightImage> variance_map,
    std::shared_ptr<Image <unsigned char>> mask, WeightImage::PixelType variance_threshold) const override;

private:
  std::array<int, 2> m_cell_size;
  std::array<int, 2> m_smoothing_box;

  WeightImageConfig::WeightType m_weight_type;
};

} // end of namespace SourceXtractor

#endif //SOURCEXTRACTORPLUSPLUS_SEBACKGROUNDLEVELANALYZER_H

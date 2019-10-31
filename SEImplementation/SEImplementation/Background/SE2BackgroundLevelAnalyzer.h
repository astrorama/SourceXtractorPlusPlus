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
 * Background
 *
 *  Created on: Oct 11, 2016
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_BACKGROUND_SE2BACKGROUNDANALYZER_H_
#define _SEIMPLEMENTATION_BACKGROUND_SE2BACKGROUNDANALYZER_H_

#include "SEFramework/Frame/Frame.h"
#include "SEFramework/Image/Image.h"
#include "SEFramework/Background/BackgroundAnalyzer.h"

#include "SEImplementation/Configuration/WeightImageConfig.h"

namespace SourceXtractor {

class SE2BackgroundLevelAnalyzer : public BackgroundAnalyzer {
public:

  SE2BackgroundLevelAnalyzer(const std::string &cell_size, const std::string &smoothing_box, const WeightImageConfig::WeightType weight_type);

  virtual ~SE2BackgroundLevelAnalyzer() = default;

  BackgroundModel analyzeBackground(
      std::shared_ptr<DetectionImage> image, std::shared_ptr<WeightImage> variance_map, std::shared_ptr<Image<unsigned char>> mask, WeightImage::PixelType variance_threshold) const override;

private:
  //SeFloat getMedian(std::shared_ptr<DetectionImageFrame> frame) const;

  BackgroundModel fromSE2Modeller(std::shared_ptr<DetectionImage> image, std::shared_ptr<WeightImage> variance_map, std::shared_ptr<Image<unsigned char>> mask, WeightImage::PixelType variance_threshold) const;
  std::vector<int>                stringToIntVec(const std::string inString, const std::string delimiters);
  std::vector<std::string>        stringSplit(const std::string inString, const std::string delimiters);

  std::vector<int> m_cell_size;
  std::vector<int> m_smoothing_box;

  WeightImageConfig::WeightType m_weight_type;
};

}

#endif /* _SEIMPLEMENTATION_BACKGROUND_SE2BACKGROUNDANALYZER_H_ */

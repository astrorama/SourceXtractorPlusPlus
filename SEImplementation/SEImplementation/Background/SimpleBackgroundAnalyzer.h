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

#ifndef _SEIMPLEMENTATION_BACKGROUND_SIMPLEBACKGROUNDANALYZER_H_
#define _SEIMPLEMENTATION_BACKGROUND_SIMPLEBACKGROUNDANALYZER_H_

#include "SEFramework/Frame/Frame.h"
#include "SEFramework/Image/Image.h"
#include "SEFramework/Background/BackgroundAnalyzer.h"

namespace SExtractor {

class SimpleBackgroundAnalyzer : public BackgroundAnalyzer {
public:

  virtual ~SimpleBackgroundAnalyzer() = default;

  //void analyzeBackground(std::shared_ptr<DetectionImageFrame> frame) const override;

  BackgroundModel analyzeBackground(
      std::shared_ptr<DetectionImage> image, std::shared_ptr<WeightImage> variance_map, std::shared_ptr<Image<unsigned char>> mask, WeightImage::PixelType variance_threshold) const override;

  static SeFloat getVariance(std::shared_ptr<DetectionImage> image);
private:
  //SeFloat getMedian(std::shared_ptr<DetectionImageFrame> frame) const;

};

}

#endif /* _SEIMPLEMENTATION_BACKGROUND_SIMPLEBACKGROUNDANALYZER_H_ */

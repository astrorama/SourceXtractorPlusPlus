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
 * BackgroundAnalyzer.h
 *
 *  Created on: Mar 28, 2017
 *      Author: mschefer
 */

#ifndef _SEFRAMEWORK_BACKGROUND_BACKGROUNDANALYZER_H_
#define _SEFRAMEWORK_BACKGROUND_BACKGROUNDANALYZER_H_

#include "SEFramework/Frame/Frame.h"
#include "SEFramework/Image/FunctionalImage.h"

namespace SourceXtractor {


class BackgroundModel {
public:

  BackgroundModel(std::shared_ptr<Image<SeFloat>> background_level, std::shared_ptr<Image<SeFloat>> background_variance, SeFloat scaling_factor) :
    m_background_level(background_level),
    m_scaling_factor(scaling_factor) {
    // make sure the variance is a positive value
    m_background_variance = FunctionalImage<SeFloat>::create(
        background_variance->getWidth(), background_variance->getHeight(),
        [background_variance](int x, int y) -> SeFloat {
      return background_variance->getValue(x,y) < 0.0 ? -1.0*background_variance->getValue(x,y) : background_variance->getValue(x,y);;
    }
    );
  }

  std::shared_ptr<Image<SeFloat>> getLevelMap() const {
    return m_background_level;
  }

  std::shared_ptr<Image<SeFloat>> getVarianceMap() const {
    return m_background_variance;
  }

  SeFloat getScalingFactor() const {
    return m_scaling_factor;
  }

private:
  std::shared_ptr<Image<SeFloat>> m_background_level;
  std::shared_ptr<Image<SeFloat>> m_background_variance;
  SeFloat m_scaling_factor;
};

class BackgroundAnalyzer {
public:

  virtual ~BackgroundAnalyzer() = default;

  virtual BackgroundModel analyzeBackground(
      std::shared_ptr<DetectionImage> image, std::shared_ptr<WeightImage> variance_map,
      std::shared_ptr<Image<unsigned char>> mask, WeightImage::PixelType variance_threshold) const = 0;
};

}


#endif /* SEFRAMEWORK_SEFRAMEWORK_BACKGROUND_BACKGROUNDANALYZER_H_ */

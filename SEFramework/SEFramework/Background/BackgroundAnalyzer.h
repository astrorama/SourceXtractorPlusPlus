/*
 * BackgroundAnalyzer.h
 *
 *  Created on: Mar 28, 2017
 *      Author: mschefer
 */

#ifndef _SEFRAMEWORK_BACKGROUND_BACKGROUNDANALYZER_H_
#define _SEFRAMEWORK_BACKGROUND_BACKGROUNDANALYZER_H_

#include "SEFramework/Frame/Frame.h"

namespace SExtractor {


class BackgroundModel {
public:

  BackgroundModel(std::shared_ptr<Image<SeFloat>> background_level, std::shared_ptr<Image<SeFloat>> background_variance,
      SeFloat scaling_factor) :
        m_background_level(background_level),
        m_background_variance(background_variance),
        m_scaling_factor(scaling_factor)
      {}

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

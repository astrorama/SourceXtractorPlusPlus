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

namespace SExtractor {

class SE2BackgroundLevelAnalyzer : public BackgroundAnalyzer {
public:

  virtual ~SE2BackgroundLevelAnalyzer() = default;

  SE2BackgroundLevelAnalyzer():m_cell_size(0), m_smoothing_box(0) {
  };

  //void analyzeBackground(std::shared_ptr<DetectionImageFrame> frame) const override;

  std::shared_ptr<Image<SeFloat>> analyzeBackground(
      std::shared_ptr<DetectionImage> image, std::shared_ptr<WeightImage> variance_map, std::shared_ptr<Image<unsigned char>> mask) const override;
  virtual void setParameters(int cell_size, int smoothing_box);

private:
  SeFloat getMedian(std::shared_ptr<DetectionImageFrame> frame) const;

  std::shared_ptr<Image<SeFloat>> fromSE2Modeller(std::shared_ptr<DetectionImage> image) const;
  std::shared_ptr<Image<SeFloat>> fromMedianValue(std::shared_ptr<DetectionImage> image) const;

  int m_cell_size;
  int m_smoothing_box;
};

}

#endif /* _SEIMPLEMENTATION_BACKGROUND_SE2BACKGROUNDANALYZER_H_ */

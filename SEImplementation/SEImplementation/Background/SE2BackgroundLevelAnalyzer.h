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

  SE2BackgroundLevelAnalyzer() {
  };

  //void analyzeBackground(std::shared_ptr<DetectionImageFrame> frame) const override;

  std::shared_ptr<Image<SeFloat>> analyzeBackground(
      std::shared_ptr<DetectionImage> image, std::shared_ptr<WeightImage> variance_map, std::shared_ptr<Image<unsigned char>> mask) const override;
  virtual void setParameters(std::string cell_size, std::string smoothing_box);

private:
  SeFloat getMedian(std::shared_ptr<DetectionImageFrame> frame) const;

  std::shared_ptr<Image<SeFloat>> fromSE2Modeller(std::shared_ptr<DetectionImage> image, std::shared_ptr<WeightImage> variance_map, std::shared_ptr<Image<unsigned char>> mask) const;
  std::shared_ptr<Image<SeFloat>> fromMedianValue(std::shared_ptr<DetectionImage> image) const;
  std::vector<int>                stringToIntVec(const std::string inString, const std::string delimiters);
  std::vector<std::string>        stringSplit(const std::string inString, const std::string delimiters);

  std::vector<int> m_cell_size;
  std::vector<int> m_smoothing_box;
};

}

#endif /* _SEIMPLEMENTATION_BACKGROUND_SE2BACKGROUNDANALYZER_H_ */

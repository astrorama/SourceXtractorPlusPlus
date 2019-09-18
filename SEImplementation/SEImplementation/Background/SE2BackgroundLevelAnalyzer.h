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

namespace SExtractor {

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

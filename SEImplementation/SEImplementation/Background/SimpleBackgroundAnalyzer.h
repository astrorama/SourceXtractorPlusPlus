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

  static SeFloat getRMSLevel(std::shared_ptr<DetectionImage> image);
private:
  //SeFloat getMedian(std::shared_ptr<DetectionImageFrame> frame) const;

};

}

#endif /* _SEIMPLEMENTATION_BACKGROUND_SIMPLEBACKGROUNDANALYZER_H_ */

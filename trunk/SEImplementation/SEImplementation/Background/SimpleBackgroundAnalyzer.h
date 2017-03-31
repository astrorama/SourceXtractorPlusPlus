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

  void analyzeBackground(std::shared_ptr<DetectionImageFrame> frame) const override;

private:

  SeFloat getMedian(std::shared_ptr<DetectionImageFrame> frame) const;
  SeFloat getRMS(std::shared_ptr<DetectionImageFrame> frame, SeFloat background_value) const;

};

}

#endif /* _SEIMPLEMENTATION_BACKGROUND_SIMPLEBACKGROUNDANALYZER_H_ */

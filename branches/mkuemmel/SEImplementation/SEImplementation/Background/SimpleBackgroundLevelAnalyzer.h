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

class SimpleBackgroundLevelAnalyzer : public BackgroundAnalyzer {
public:

  virtual ~SimpleBackgroundLevelAnalyzer() = default;

  void analyzeBackground(std::shared_ptr<DetectionImageFrame> frame) const override;

private:
  SeFloat getMedian(std::shared_ptr<DetectionImageFrame> frame) const;
};

}

#endif /* _SEIMPLEMENTATION_BACKGROUND_SIMPLEBACKGROUNDANALYZER_H_ */

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

class BackgroundAnalyzer {
public:

  virtual ~BackgroundAnalyzer() = default;

  virtual void analyzeBackground(std::shared_ptr<DetectionImageFrame> frame) const = 0;
};

}


#endif /* SEFRAMEWORK_SEFRAMEWORK_BACKGROUND_BACKGROUNDANALYZER_H_ */

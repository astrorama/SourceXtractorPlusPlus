/*
 * BackgroundSubtract.h
 *
 *  Created on: Sep 12, 2016
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_SEGMENTATION_BACKGROUNDSUBTRACT_H_
#define _SEIMPLEMENTATION_SEGMENTATION_BACKGROUNDSUBTRACT_H_

#include "SEFramework/Image/ImageProcessing.h"

namespace SExtractor {

class BackgroundSubtract : public DetectionImageProcessing {
public:
  BackgroundSubtract(SeFloat background_value) : m_background_value(background_value) {}

  virtual std::shared_ptr<DetectionImage> processImage(std::shared_ptr<DetectionImage> image) override;

private:
  SeFloat m_background_value;
};

}

#endif /* _SEIMPLEMENTATION_SEGMENTATION_BACKGROUNDSUBTRACT_H_ */

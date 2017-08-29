/*
 * SimpleBackgroundRMSAnalyzer.cpp
 *
 *  Created on: Apr 25, 2017
 *      Author: mschefer
 */

#include <memory>

#include "SEFramework/Image/VectorImage.h"

#include "SEImplementation/Background/SimpleBackgroundRMSAnalyzer.h"

namespace SExtractor {

void SimpleBackgroundRMSAnalyzer::analyzeBackground(std::shared_ptr<DetectionImageFrame> frame) const {
  auto rms = getRMS(frame);
  frame->setBackgroundRMS(rms);
}

SeFloat SimpleBackgroundRMSAnalyzer::getRMS(std::shared_ptr<DetectionImageFrame> frame) const {

  // Note: We compute the RMS by only taking into consideration pixels
  // below the background value.

  auto detection_image = frame->getSubtractedImage();

  double rms = 0;
  int pixels = 0;
  for (int y=0; y < detection_image->getHeight(); y++) {
    for (int x=0; x < detection_image->getWidth(); x++) {
      auto value = detection_image->getValue(x, y);
      if (value < 0) {
        rms += value * value;
        pixels++;
      }
    }
  }
  if (pixels > 0) {
    rms /= pixels;
  }

  return sqrt(rms);
}

}

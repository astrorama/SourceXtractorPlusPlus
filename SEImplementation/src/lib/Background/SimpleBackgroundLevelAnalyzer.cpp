/*
 * Background.cpp
 *
 *  Created on: Oct 11, 2016
 *      Author: mschefer
 */

#include <memory>
#include <algorithm>

#include "SEFramework/Image/VectorImage.h"

#include "SEImplementation/Background/SimpleBackgroundLevelAnalyzer.h"

namespace SExtractor {


void SimpleBackgroundLevelAnalyzer::analyzeBackground(std::shared_ptr<DetectionImageFrame> frame) const {
  frame->setBackgroundLevel(getMedian(frame));
}

SeFloat SimpleBackgroundLevelAnalyzer::getMedian(std::shared_ptr<DetectionImageFrame> frame) const {
  auto detection_image = frame->getOriginalImage();
  if (detection_image != nullptr) {
    auto image_copy = VectorImage<DetectionImage::PixelType>::create(*detection_image);
    std::sort(image_copy->getData().begin(), image_copy->getData().end());
    return image_copy->getData()[image_copy->getData().size()/2];
  }
  return 0;
}

}


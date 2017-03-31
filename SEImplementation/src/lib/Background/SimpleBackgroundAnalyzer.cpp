/*
 * Background.cpp
 *
 *  Created on: Oct 11, 2016
 *      Author: mschefer
 */
#include <memory>
#include <algorithm>

#include "SEFramework/Image/VectorImage.h"

#include <iostream>

#include "SEImplementation/Background/SimpleBackgroundAnalyzer.h"


namespace SExtractor {


void SimpleBackgroundAnalyzer::analyzeBackground(std::shared_ptr<DetectionImageFrame> frame) const {
  auto median = getMedian(frame);
  auto rms = getRMS(frame, median);

  frame->setBackgroundLevel(median);
  frame->setBackgroundRMS(rms);
  frame->setDetectionThreshold(rms * 1.5);
}

SeFloat SimpleBackgroundAnalyzer::getMedian(std::shared_ptr<DetectionImageFrame> frame) const {
  auto detection_image = frame->getOriginalImage();
  if (detection_image != nullptr) {
    auto image_copy = std::make_shared<VectorImage<DetectionImage::PixelType>>(*detection_image);
    std::sort(image_copy->getData().begin(), image_copy->getData().end());
    return image_copy->getData()[image_copy->getData().size()/2];
  }
  return 0;
}

SeFloat SimpleBackgroundAnalyzer::getRMS(std::shared_ptr<DetectionImageFrame> frame, SeFloat background_value) const {

  // Note: We compute the RMS by only taking into consideration pixels
  // below the background value.

  auto detection_image = frame->getOriginalImage();

  double rms = 0;
  int pixels = 0;
  for (int y=0; y < detection_image->getHeight(); y++) {
    for (int x=0; x < detection_image->getWidth(); x++) {
      auto value = detection_image->getValue(x, y);
      if (value < background_value) {
        auto diff = value - background_value;
        rms += diff * diff;
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


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

std::shared_ptr<Image<SeFloat>> SimpleBackgroundRMSAnalyzer::analyzeBackground(
    std::shared_ptr<DetectionImage> image, std::shared_ptr<WeightImage> variance_map, std::shared_ptr<Image<unsigned char>> mask, WeightImage::PixelType variance_threshold) const {

  // Note: We compute the RMS by only taking into consideration pixels
  // below the background value.

  double rms = 0;
  int pixels = 0;
  for (int y=0; y < image->getHeight(); y++) {
    for (int x=0; x < image->getWidth(); x++) {
      auto value = image->getValue(x, y);
      if (value < 0) {
        rms += value * value;
        pixels++;
      }
    }
  }
  if (pixels > 0) {
    rms /= pixels;
  }
  return ConstantImage<SeFloat>::create(image->getWidth(), image->getHeight(), sqrt(rms));
}

}

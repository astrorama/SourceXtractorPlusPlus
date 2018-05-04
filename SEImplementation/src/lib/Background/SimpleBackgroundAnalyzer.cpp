/*
 * Background.cpp
 *
 *  Created on: Oct 11, 2016
 *      Author: mschefer
 */

#include "SEImplementation/Background/SimpleBackgroundAnalyzer.h"

#include <memory>
#include <algorithm>

#include <iostream>

#include "SEFramework/Image/ConstantImage.h"
#include "SEFramework/Image/VectorImage.h"


namespace SExtractor {

BackgroundModel SimpleBackgroundAnalyzer::analyzeBackground(
    std::shared_ptr<DetectionImage> image,
    std::shared_ptr<WeightImage> /*variance_map*/, std::shared_ptr<Image<unsigned char>> /*mask*/,
    WeightImage::PixelType /*variance_threshold*/) const {

  // FIXME we use a VectorImage the same size as input which won't allow larger than memory images to be processed

  auto image_copy = VectorImage<DetectionImage::PixelType>::create(*image);
  std::sort(image_copy->getData().begin(), image_copy->getData().end());
  std::cout << "Using the SimpleBackgroundLeverAnalyzer" << std::endl<< std::endl<< std::endl;

  auto background_level = image_copy->getData()[image_copy->getData().size()/2]; // the median
  auto background_level_map = ConstantImage<SeFloat>::create(image->getWidth(), image->getHeight(), background_level);

  auto subtracted_image = SubtractImage<SeFloat>::create(image, background_level_map);

  auto background_variance = getVariance(subtracted_image);
  auto background_variance_map = ConstantImage<SeFloat>::create(image->getWidth(), image->getHeight(), background_variance);

  return BackgroundModel(background_level_map, background_variance_map, 99999); // FIXME
}


SeFloat SimpleBackgroundAnalyzer::getVariance(std::shared_ptr<DetectionImage> image) {
  // Note: We compute the RMS by only taking into consideration pixels
  // below the background value.

  double variance = 0;
  int pixels = 0;
  for (int y=0; y < image->getHeight(); y++) {
    for (int x=0; x < image->getWidth(); x++) {
      auto value = image->getValue(x, y);
      if (value < 0) {
        variance += value * value;
        pixels++;
      }
    }
  }

  if (pixels > 0) {
    variance /= pixels;
  }

  return variance;
}

}


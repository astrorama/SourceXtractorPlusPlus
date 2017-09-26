/*
 * Background.cpp
 *
 *  Created on: Oct 11, 2016
 *      Author: mschefer
 */

#include <memory>
#include <algorithm>

#include "SEFramework/Image/ConstantImage.h"
#include "SEFramework/Image/VectorImage.h"

#include "SEImplementation/Background/SimpleBackgroundLevelAnalyzer.h"

namespace SExtractor {

std::shared_ptr<Image<SeFloat>> SimpleBackgroundLevelAnalyzer::analyzeBackground(
    std::shared_ptr<DetectionImage> image,
    std::shared_ptr<WeightImage> variance_map, std::shared_ptr<Image<unsigned char>> mask) const {

  auto image_copy = VectorImage<DetectionImage::PixelType>::create(*image);
  std::sort(image_copy->getData().begin(), image_copy->getData().end());

  return ConstantImage<SeFloat>::create(image->getWidth(), image->getHeight(), image_copy->getData()[image_copy->getData().size()/2]);
}

}


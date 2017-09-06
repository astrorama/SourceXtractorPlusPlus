/*
 * BackgroundSubtract.cpp
 *
 *  Created on: Sep 12, 2016
 *      Author: mschefer
 */

#include "SEFramework/Image/SubtractImage.h"
#include "SEImplementation/Segmentation/BackgroundSubtract.h"


namespace SExtractor {

std::shared_ptr<DetectionImage> BackgroundSubtract::processImage(std::shared_ptr<DetectionImage> image) const {
  return SubtractImage<DetectionImage::PixelType>::create(image, m_background_value);
}

}


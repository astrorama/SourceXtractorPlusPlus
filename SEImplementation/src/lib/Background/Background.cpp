/*
 * Background.cpp
 *
 *  Created on: Oct 11, 2016
 *      Author: mschefer
 */
#include <memory>
#include <algorithm>

#include "SEFramework/Image/VectorImage.h"

#include "SEImplementation/Background/Background.h"

#include <iostream>
namespace SExtractor {


SeFloat Background::getMedian() const {
  if (m_detection_image != nullptr) {
    auto image_copy = std::make_shared<VectorImage<DetectionImage::PixelType>>(*m_detection_image);
    std::sort(image_copy->getData().begin(), image_copy->getData().end());
    return image_copy->getData()[image_copy->getData().size()/2];
  }
  return 0;
}

SeFloat Background::getRMS(SeFloat background_value) const {

  // Note: We compute the RMS by only taking into consideration pixels
  // below the background value.

  double rms = 0;
  int pixels = 0;
  for (int y=0; y < m_detection_image->getHeight(); y++) {
    for (int x=0; x < m_detection_image->getWidth(); x++) {
      auto value = m_detection_image->getValue(x, y);
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


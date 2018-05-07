/*
 * BackgroundConvolution.cpp
 *
 *  Created on: Oct 12, 2016
 *      Author: mschefer
 */

#include "SEImplementation/Segmentation/BackgroundConvolution.h"

namespace SExtractor {

std::shared_ptr<DetectionImage> BackgroundConvolution::processImage(std::shared_ptr<DetectionImage> image, std::shared_ptr<DetectionImage> variance, SeFloat threshold) const {
  auto result_image = BufferedImage<DetectionImage::PixelType>::create(std::make_shared<BgConvolutionImageSource>(image, variance, threshold, m_convolution_filter));
  return result_image;
}

void BackgroundConvolution::normalize() {
  double total = 0;

  for (int y = 0; y < m_convolution_filter->getHeight(); y++) {
    for (int x = 0; x < m_convolution_filter->getWidth(); x++) {
      total += m_convolution_filter->getValue(x, y);
    }
  }

  for (int y = 0; y < m_convolution_filter->getHeight(); y++) {
    for (int x = 0; x < m_convolution_filter->getWidth(); x++) {
      m_convolution_filter->setValue(x, y, m_convolution_filter->getValue(x, y) / total);
    }
  }

}



}

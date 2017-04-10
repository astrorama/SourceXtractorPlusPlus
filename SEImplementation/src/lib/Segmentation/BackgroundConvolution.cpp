/*
 * BackgroundConvolution.cpp
 *
 *  Created on: Oct 12, 2016
 *      Author: mschefer
 */

#include "SEFramework/Image/VectorImage.h"

#include "SEImplementation/Segmentation/BackgroundConvolution.h"

namespace SExtractor {

std::shared_ptr<DetectionImage> BackgroundConvolution::processImage(std::shared_ptr<DetectionImage> image) const {

  auto result_image = std::make_shared<VectorImage<DetectionImage::PixelType>>(image->getWidth(), image->getHeight());

  int hx = m_convolution_filter->getWidth() / 2;
  int hy = m_convolution_filter->getHeight() / 2;

  for (int y = 0; y < image->getHeight(); y++) {
    for (int x = 0; x < image->getWidth(); x++) {

      DetectionImage::PixelType total = 0;

      for (int cy = 0; cy < m_convolution_filter->getHeight(); cy++) {
        for (int cx = 0; cx < m_convolution_filter->getWidth(); cx++) {

          auto x2 = x + cx - hx;
          auto y2 = y + cy - hy;

// alternative way to handle borders
//          x2 = std::min(image->getWidth() - 1, std::max(0, x2));
//          y2 = std::min(image->getHeight() - 1, std::max(0, y2));
//
//          auto weight = m_convolution_filter->getValue(cx, cy);
//          total += image->getValue(x2, y2) * weight;
//          total_weight += weight;

          if (x2 >= 0 && x2 < image->getWidth() && y2 >= 0 && y2 < image->getHeight()) {
            total += image->getValue(x2, y2) * m_convolution_filter->getValue(cx, cy);
          }

          result_image->setValue(x, y, total);
        }
      }
    }
  }

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

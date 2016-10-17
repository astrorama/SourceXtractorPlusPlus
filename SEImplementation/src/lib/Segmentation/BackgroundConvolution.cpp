/*
 * BackgroundConvolution.cpp
 *
 *  Created on: Oct 12, 2016
 *      Author: mschefer
 */

#include "SEFramework/Image/VectorImage.h"

#include "SEImplementation/Segmentation/BackgroundConvolution.h"

namespace SExtractor {

std::shared_ptr<DetectionImage> BackgroundConvolution::processImage(std::shared_ptr<DetectionImage> image) {

  auto result_image = std::make_shared<VectorImage<DetectionImage::PixelType>>(image->getWidth(), image->getHeight());

  int hx = m_convolution_filter->getWidth() / 2;
  int hy = m_convolution_filter->getHeight() / 2;

  for (int y = 0; y < image->getHeight(); y++) {
    for (int x = 0; x < image->getWidth(); x++) {

      DetectionImage::PixelType total = 0;
      SeFloat total_weight = 0;

      for (int cy = 0; cy < m_convolution_filter->getHeight(); cy++) {
        for (int cx = 0; cx < m_convolution_filter->getWidth(); cx++) {

          auto x2 = x + cx - hx;
          auto y2 = y + cy - hy;

          if (x2 >= 0 && x2 < image->getWidth() && y2 >= 0 && y2 < image->getHeight()) {
            auto weight = m_convolution_filter->getValue(cx, cy);
            total += image->getValue(x2, y2) * weight;
            total_weight += weight;
          }

          result_image->setValue(x, y, total / total_weight);
        }
      }
    }
  }

  return result_image;
}


}

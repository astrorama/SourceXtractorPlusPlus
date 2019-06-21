/*
 * BgConvolutionImageSource.cpp
 *
 *  Created on: Jun 12, 2019
 *      Author: Alejandro Alvarez
 *      Refactored out from: BackgroundConvolution.h
 */

#include "SEImplementation/Segmentation/BgConvolutionImageSource.h"

namespace SExtractor {


BgConvolutionImageSource::BgConvolutionImageSource(std::shared_ptr<Image<DetectionImage::PixelType>> image,
                                                   std::shared_ptr<DetectionImage> variance, SeFloat threshold,
                                                   std::shared_ptr<VectorImage<SeFloat>> kernel)
  : ProcessingImageSource<DetectionImage::PixelType>(image),
    m_variance(variance), m_threshold(threshold) {
  m_kernel = VectorImage<SeFloat>::create(MirrorImage<SeFloat>::create(kernel));
}

std::string BgConvolutionImageSource::getRepr() const {
  return "BgConvolutionImageSource(" + getImageRepr() + ")";
}

void BgConvolutionImageSource::generateTile(std::shared_ptr<Image<DetectionImage::PixelType>> image,
                                            ImageTile<DetectionImage::PixelType>& tile,
                                            int x, int y, int width, int height) const {
  int hx = m_kernel->getWidth() / 2;
  int hy = m_kernel->getHeight() / 2;

  for (int iy = y; iy < y + height; iy++) {
    for (int ix = x; ix < x + width; ix++) {
      DetectionImage::PixelType total = 0;
      DetectionImage::PixelType total_ignoring_threshold = 0;
      DetectionImage::PixelType conv_weight = 0;
      DetectionImage::PixelType conv_weight_ignoring_threshold = 0;

      if (m_variance->getValue(ix, iy) < m_threshold) {

        for (int cy = 0; cy < m_kernel->getHeight(); cy++) {
          for (int cx = 0; cx < m_kernel->getWidth(); cx++) {

            auto x2 = ix + cx - hx;
            auto y2 = iy + cy - hy;

            if (x2 >= 0 && x2 < image->getWidth() && y2 >= 0 && y2 < image->getHeight()) {
              if (m_variance->getValue(x2, y2) < m_threshold) {
                total += image->getValue(x2, y2) * m_kernel->getValue(cx, cy);
                conv_weight += m_kernel->getValue(cx, cy);
              }
              total_ignoring_threshold += image->getValue(x2, y2) * m_kernel->getValue(cx, cy);
              conv_weight_ignoring_threshold += m_kernel->getValue(cx, cy);
            }
            if (conv_weight > 0) {
              tile.getImage()->setValue(ix - x, iy - y, total / conv_weight);
            } else {
              tile.getImage()->setValue(ix - x, iy - y, total_ignoring_threshold / conv_weight_ignoring_threshold);
            }
          }
        }
      } else {
        tile.getImage()->setValue(ix - x, iy - y, 0);
      }
    }
  }
}


} // end namespace SExtractor


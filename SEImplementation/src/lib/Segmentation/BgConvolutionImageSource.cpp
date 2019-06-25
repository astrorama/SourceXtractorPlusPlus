/*
 * BgConvolutionImageSource.cpp
 *
 *  Created on: Jun 12, 2019
 *      Author: Alejandro Alvarez
 *      Refactored out from: BackgroundConvolution.h
 */

#include "SEImplementation/Segmentation/BgConvolutionImageSource.h"
#include "SEFramework/Image/FunctionalImage.h"

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
                                            int start_x, int start_y, int width, int height) const {
  const int hx = m_kernel->getWidth() / 2;
  const int hy = m_kernel->getHeight() / 2;
  const int clip_x = std::max(start_x - hx, 0);
  const int clip_y = std::max(start_y - hy, 0);
  const int clip_w = std::min(width + hx * 2, image->getWidth() - clip_x);
  const int clip_h = std::min(height + hy * 2, image->getHeight() - clip_y);

  // "Materialize" the image and variance
  auto image_chunk = image->getChunk(clip_x, clip_y, clip_w, clip_h);
  auto variance_chunk = m_variance->getChunk(clip_x, clip_y, clip_w, clip_h);

  // Convolve both and copy out to the tile
  const int off_x = start_x - clip_x;
  const int off_y = start_y - clip_y;

  for (int iy = 0; iy < height; ++iy) {
    for (int ix = 0; ix < width; ++ix) {
      DetectionImage::PixelType total = 0.;
      DetectionImage::PixelType conv_weight = 0.;

      if (variance_chunk->getValue(ix + off_x, iy + off_y) < m_threshold) {
        for (int cy = 0; cy < m_kernel->getHeight(); ++cy) {
          for (int cx = 0; cx < m_kernel->getWidth(); ++cx) {
            int x2 = ix + cx - hx;
            int y2 = iy + cy - hy;
            int clip_x2 = x2 + off_x;
            int clip_y2 = y2 + off_y;

            if (clip_x2 >= 0 && clip_x2 < clip_w && clip_y2 >= 0 && clip_y2 < clip_h) {
              if (variance_chunk->getValue(clip_x2, clip_y2) < m_threshold) {
                total += image_chunk->getValue(clip_x2, clip_y2) * m_kernel->getValue(cx, cy);
                conv_weight += m_kernel->getValue(cx,cy);
              }
            }
          }
        }
        // Note that because of the conditional, at least the center pixel is below the threshold,
        // so checking for conv_weight > 0 is redundant
        tile.getImage()->setValue(ix, iy, total / conv_weight);
      }
      else {
        tile.getImage()->setValue(ix, iy, 0.);
      }
    }
  }
}


} // end namespace SExtractor


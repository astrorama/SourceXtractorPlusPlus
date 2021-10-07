/** Copyright © 2019 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3.0 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */
/*
 * BgConvolutionImageSource.cpp
 *
 *  Created on: Jun 12, 2019
 *      Author: Alejandro Alvarez
 *      Refactored out from: BackgroundConvolution.h
 */

#include "SEImplementation/Segmentation/BgConvolutionImageSource.h"
#include "SEFramework/Image/FunctionalImage.h"

namespace SourceXtractor {


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

static std::tuple<float, float> applyKernel(const VectorImage<SeFloat>& kernel, const VectorImage<SeFloat>& image_chunk,
                                            const VectorImage<SeFloat>& variance_chunk, int start_x, int start_y, int clip_w,
                                            int clip_h, SeFloat threshold) {
  DetectionImage::PixelType total = 0.;
  DetectionImage::PixelType conv_weight = 0.;

  for (int cy = 0; cy < kernel.getHeight(); ++cy) {
    for (int cx = 0; cx < kernel.getWidth(); ++cx) {
      int clip_x2 = start_x + cx;
      int clip_y2 = start_y + cy;

      if (clip_x2 >= 0 && clip_x2 < clip_w && clip_y2 >= 0 && clip_y2 < clip_h) {
        if (variance_chunk.getValue(clip_x2, clip_y2) < threshold) {
          total += image_chunk.getValue(clip_x2, clip_y2) * kernel.getValue(cx, cy);
          conv_weight += kernel.getValue(cx, cy);
        }
      }
    }
  }

  return std::make_pair(total, conv_weight);
}

void BgConvolutionImageSource::generateTile(const std::shared_ptr<Image<DetectionImage::PixelType>>& image,
                                            ImageTileWithType<DetectionImage::PixelType>& tile, int start_x,
                                            int start_y, int width, int height) const {
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
  auto& tile_image = *tile.getImage();
  for (int iy = 0; iy < height; ++iy) {
    for (int ix = 0; ix < width; ++ix) {
      if (variance_chunk->getValue(ix + off_x, iy + off_y) < m_threshold) {
        DetectionImage::PixelType total = 0.;
        DetectionImage::PixelType conv_weight = 0.;

        std::tie(total, conv_weight) = applyKernel(*m_kernel, *image_chunk, *variance_chunk,
                                                   ix - hx + off_x, iy - hy + off_y, clip_w, clip_h,
                                                   m_threshold);

        // Note that because of the conditional, at least the center pixel is below the threshold,
        // so checking for conv_weight > 0 is redundant
        tile_image.setValue(ix, iy, total / conv_weight);
      }
      else {
        tile_image.setValue(ix, iy, 0.);
      }
    }
  }
}


} // end namespace SourceXtractor


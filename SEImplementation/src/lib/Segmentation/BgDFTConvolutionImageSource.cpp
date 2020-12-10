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
 * BgDFTConvolutionImageSource.h
 *
 *  Created on: Jun 12, 2019
 *      Author: Alejandro Alvarez
 *      Refactored out from: BackgroundConvolution.h
 */

#include "SEImplementation/Segmentation/BgDFTConvolutionImageSource.h"
#include "SEFramework/Image/FunctionalImage.h"
#include "SEFramework/Image/MaskedImage.h"

namespace SourceXtractor {


BgDFTConvolutionImageSource::BgDFTConvolutionImageSource(std::shared_ptr<Image<DetectionImage::PixelType>> image,
                                                         std::shared_ptr<DetectionImage> variance, SeFloat threshold,
                                                         std::shared_ptr<VectorImage<SeFloat>> kernel)
  : ProcessingImageSource<DetectionImage::PixelType>(image),
    m_variance(variance), m_threshold(threshold), m_convolution(kernel) {
}

std::string BgDFTConvolutionImageSource::getRepr() const {
  return "BgDFTConvolutionImageSource(" + getImageRepr() + ")";
}

void BgDFTConvolutionImageSource::generateTile(const std::shared_ptr<Image<DetectionImage::PixelType>>& image,
                                               ImageTile& tile, int start_x, int start_y, int width, int height) const {
  int hx = m_convolution.getWidth() / 2;
  int hy = m_convolution.getHeight() / 2;
  int clip_x = std::max(start_x - hx, 0);
  int clip_y = std::max(start_y - hy, 0);
  int clip_w = std::min(width + hx * 2, image->getWidth() - clip_x);
  int clip_h = std::min(height + hy * 2, image->getHeight() - clip_y);

  // Clip the image and variance map to the given size, accounting for the margins for the convolution
  auto clipped_img = FunctionalImage<DetectionImage::PixelType>::create(
    clip_w, clip_h, [image, clip_x, clip_y](int x, int y) {
      return image->getValue(clip_x + x, clip_y + y);
    }
  );
  auto clipped_variance = FunctionalImage<DetectionImage::PixelType>::create(
    clip_w, clip_h, [this, clip_x, clip_y](int x, int y) {
      return m_variance->getValue(clip_x + x, clip_y + y);
    }
  );

  // Get the mask
  // For instance, with a threshold of 0.5
  // Variance     Mask       Negative
  // 1  1  1     0  0  0     1  1  1
  // 1  0  1     0  1  0     1  0  1
  // 1  1  1     0  0  0     1  1  1
  auto mask = FunctionalImage<DetectionImage::PixelType>::create(
    clipped_variance->getWidth(), clipped_variance->getHeight(),
    [this, clipped_variance](int x, int y) -> DetectionImage::PixelType {
      return clipped_variance->getValue(x, y) < m_threshold;
    }
  );

  // Get the image masking out values where the variance is greater than the threshold
  auto masked_img = MaskedImage<DetectionImage::PixelType, DetectionImage::PixelType, std::equal_to>::create(
    clipped_img, mask, 0., 0.);

  // Convolve the masked image, padding with 0
  auto conv_masked = VectorImage<DetectionImage::PixelType>::create(masked_img);
  m_convolution.convolve(conv_masked);

  // Convolve the mask
  // This gives us in each cell the sum of the kernel values that have been used,
  // so we can divide the previous convolution.
  auto conv_mask = VectorImage<DetectionImage::PixelType>::create(mask);
  m_convolution.convolve(conv_mask);

  // Copy out the value of the convolved image, divided by the negative mask, applying
  // again the mask to the convolved result
  int off_x = start_x - clip_x;
  int off_y = start_y - clip_y;
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      if (mask->getValue(x + off_x, y + off_y)) {
        tile.setValue(
          x + start_x, y + start_y,
          conv_masked->getValue(x + off_x, y + off_y) / conv_mask->getValue(x + off_x, y + off_y)
        );
      } else {
        tile.setValue(x + start_x, y + start_y, 0);
      }
    }
  }
}

} // end namespace SourceXtractor


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

#ifndef _SEIMPLEMENTATION_SEGMENTATION_BGDFTCONVOLUTIONIMAGESOURCE_H_
#define _SEIMPLEMENTATION_SEGMENTATION_BGDFTCONVOLUTIONIMAGESOURCE_H_

#include "SEFramework/Convolution/DFT.h"
#include "SEFramework/Image/PaddedImage.h"
#include "SEFramework/Image/VectorImage.h"
#include "SEFramework/Image/ProcessingImageSource.h"

namespace SourceXtractor {
/**
 * Implement an image source using direct convolution of the filter over the image.
 * This approach is normally faster for big kernels
 */
class BgDFTConvolutionImageSource : public ProcessingImageSource<DetectionImage::PixelType> {
public:
  BgDFTConvolutionImageSource(std::shared_ptr<Image<DetectionImage::PixelType>> image,
                              std::shared_ptr<DetectionImage> variance, SeFloat threshold,
                              std::shared_ptr<VectorImage<SeFloat>> kernel);

protected:

  // Constant padded
  using PaddedType = PaddedImage<DetectionImage::PixelType>;

  // Discrete Fourier Transform convolution, which is faster for big kernels
  using ConvolutionType = DFTConvolution<DetectionImage::PixelType, PaddedType>;

  std::string getRepr() const override;

  void generateTile(std::shared_ptr<Image<DetectionImage::PixelType>> image, ImageTile<DetectionImage::PixelType>& tile,
                    int start_x, int start_y, int width, int height) const override;

private:
  std::shared_ptr<DetectionImage> m_variance;
  DetectionImage::PixelType m_threshold;
  ConvolutionType m_convolution;
};

} // end namespace SourceXtractor

#endif // _SEIMPLEMENTATION_SEGMENTATION_BGDFTCONVOLUTIONIMAGESOURCE_H_

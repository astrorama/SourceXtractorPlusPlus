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

namespace SExtractor {
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

} // end namespace SExtractor

#endif // _SEIMPLEMENTATION_SEGMENTATION_BGDFTCONVOLUTIONIMAGESOURCE_H_

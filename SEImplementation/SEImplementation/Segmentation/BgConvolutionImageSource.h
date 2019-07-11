/*
 * BgConvolutionImageSource.h
 *
 *  Created on: Jun 12, 2019
 *      Author: Alejandro Alvarez
 *      Refactored out from: BackgroundConvolution.h
 */

#ifndef _SEIMPLEMENTATION_SEGMENTATION_BGCONVOLUTIONIMAGESOURCE_H_
#define _SEIMPLEMENTATION_SEGMENTATION_BGCONVOLUTIONIMAGESOURCE_H_

#include "SEFramework/Image/MirrorImage.h"
#include "SEFramework/Image/VectorImage.h"
#include "SEFramework/Image/ProcessingImageSource.h"

namespace SExtractor {

/**
 * Implement an image source using direct convolution of the filter over the image.
 * This approach is normally faster for small kernels
 */
class BgConvolutionImageSource : public ProcessingImageSource<DetectionImage::PixelType> {
public:
  BgConvolutionImageSource(std::shared_ptr<Image<DetectionImage::PixelType>> image,
                           std::shared_ptr<DetectionImage> variance, SeFloat threshold,
                           std::shared_ptr<VectorImage<SeFloat>> kernel);

protected:

  std::string getRepr() const override;

  void generateTile(std::shared_ptr<Image<DetectionImage::PixelType>> image, ImageTile<DetectionImage::PixelType>& tile,
                    int start_x, int start_y, int width, int height) const override;

private:
  std::shared_ptr<DetectionImage> m_variance;
  SeFloat m_threshold;
  std::shared_ptr<VectorImage<SeFloat>> m_kernel;
};

} // end namespace SExtractor

#endif // _SEIMPLEMENTATION_SEGMENTATION_BGCONVOLUTIONIMAGESOURCE_H_

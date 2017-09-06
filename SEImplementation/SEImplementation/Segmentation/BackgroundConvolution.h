/*
 * BackgroundConvolution.h
 *
 *  Created on: Oct 12, 2016
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_SEGMENTATION_BACKGROUNDCONVOLUTION_H_
#define _SEIMPLEMENTATION_SEGMENTATION_BACKGROUNDCONVOLUTION_H_

#include "SEFramework/Image/VectorImage.h"
#include "SEFramework/Image/ImageProcessing.h"

namespace SExtractor {

class BackgroundConvolution : public DetectionImageProcessing {
public:
  BackgroundConvolution(std::shared_ptr<Image<SeFloat>> convolution_filter, bool must_normalize)
    : m_convolution_filter(VectorImage<SeFloat>::create(*convolution_filter))
  {
    if (must_normalize) {
      normalize();
    }
  }

  virtual std::shared_ptr<DetectionImage> processImage(std::shared_ptr<DetectionImage> image) const override;

private:
  void normalize();

  std::shared_ptr<VectorImage<SeFloat>> m_convolution_filter;
};

}

#endif /* _SEIMPLEMENTATION_SEGMENTATION_BACKGROUNDCONVOLUTION_H_ */

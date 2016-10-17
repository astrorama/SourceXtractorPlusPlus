/*
 * BackgroundConvolution.h
 *
 *  Created on: Oct 12, 2016
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_SEGMENTATION_BACKGROUNDCONVOLUTION_H_
#define _SEIMPLEMENTATION_SEGMENTATION_BACKGROUNDCONVOLUTION_H_

#include "SEFramework/Image/ImageProcessing.h"

namespace SExtractor {

class BackgroundConvolution : public DetectionImageProcessing {
public:
  BackgroundConvolution(std::shared_ptr<Image<SeFloat>> convolution_filter) : m_convolution_filter(convolution_filter) {}

  virtual std::shared_ptr<DetectionImage> processImage(std::shared_ptr<DetectionImage> image) override;

private:
  std::shared_ptr<Image<SeFloat>> m_convolution_filter;
};

}

#endif /* _SEIMPLEMENTATION_SEGMENTATION_BACKGROUNDCONVOLUTION_H_ */

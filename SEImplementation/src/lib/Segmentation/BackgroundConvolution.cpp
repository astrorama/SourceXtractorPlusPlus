/*
 * BackgroundConvolution.cpp
 *
 *  Created on: Oct 12, 2016
 *      Author: mschefer
 */

#include "SEImplementation/Segmentation/BackgroundConvolution.h"
#include <ElementsKernel/Logging.h>

namespace SExtractor {

Elements::Logging logger = Elements::Logging::getLogger("Segmentation");

std::shared_ptr<DetectionImage>
BackgroundConvolution::processImage(std::shared_ptr<DetectionImage> image, std::shared_ptr<DetectionImage> variance,
                                    SeFloat threshold) const {

  if (m_convolution_filter->getWidth() > 5) {
    logger.debug() << "Using DFT algorithm for the image convolution";
    return BufferedImage<DetectionImage::PixelType>::create(
      std::make_shared<BgDFTConvolutionImageSource>(image, variance, threshold, m_convolution_filter)
    );
  }
  logger.debug() << "Using direct algorithm for the image convolution";
  return BufferedImage<DetectionImage::PixelType>::create(
    std::make_shared<BgConvolutionImageSource>(image, variance, threshold, m_convolution_filter)
  );
}

void BackgroundConvolution::normalize() {
  double total = 0;

  for (int y = 0; y < m_convolution_filter->getHeight(); y++) {
    for (int x = 0; x < m_convolution_filter->getWidth(); x++) {
      total += m_convolution_filter->getValue(x, y);
    }
  }

  for (int y = 0; y < m_convolution_filter->getHeight(); y++) {
    for (int x = 0; x < m_convolution_filter->getWidth(); x++) {
      m_convolution_filter->setValue(x, y, m_convolution_filter->getValue(x, y) / total);
    }
  }

}


}

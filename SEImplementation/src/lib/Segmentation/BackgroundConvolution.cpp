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
 * BackgroundConvolution.cpp
 *
 *  Created on: Oct 12, 2016
 *      Author: mschefer
 */

#include "SEImplementation/Segmentation/BgConvolutionImageSource.h"
#include "SEImplementation/Segmentation/BgDFTConvolutionImageSource.h"
#include "SEImplementation/Segmentation/BackgroundConvolution.h"
#include <ElementsKernel/Logging.h>

namespace SourceXtractor {

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

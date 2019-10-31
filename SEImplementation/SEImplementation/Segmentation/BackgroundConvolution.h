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
 * BackgroundConvolution.h
 *
 *  Created on: Oct 12, 2016
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_SEGMENTATION_BACKGROUNDCONVOLUTION_H_
#define _SEIMPLEMENTATION_SEGMENTATION_BACKGROUNDCONVOLUTION_H_

#include "SEUtils/Types.h"
#include "SEFramework/Image/VectorImage.h"
#include "SEFramework/Frame/Frame.h"

namespace SourceXtractor {

/**
 * BackgroundConvolution filter
 */
class BackgroundConvolution : public DetectionImageFrame::ImageFilter {

public:
  BackgroundConvolution(std::shared_ptr<Image<SeFloat>> convolution_filter, bool must_normalize)
    : m_convolution_filter(VectorImage<SeFloat>::create(*convolution_filter)) {
    if (must_normalize) {
      normalize();
    }
  }

  std::shared_ptr<DetectionImage>
  processImage(std::shared_ptr<DetectionImage> image, std::shared_ptr<DetectionImage> variance,
               SeFloat threshold) const;

private:
  void normalize();

  std::shared_ptr<VectorImage<SeFloat>> m_convolution_filter;
};

}

#endif /* _SEIMPLEMENTATION_SEGMENTATION_BACKGROUNDCONVOLUTION_H_ */

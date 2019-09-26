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
 * @file SEFramework/Convolution/OpenCVConvolution.h
 * @date 17/09/18
 * @author aalvarez
 */

#ifndef _SEFRAMEWORK_CONVOLUTION_OPENCVCONVOLUTION_H
#define _SEFRAMEWORK_CONVOLUTION_OPENCVCONVOLUTION_H

#include "SEFramework/Image/VectorImage.h"
#include <opencv2/opencv.hpp>

namespace SExtractor {

class OpenCVConvolution {
public:
  OpenCVConvolution(std::shared_ptr<const VectorImage<SeFloat>> img)
    : m_kernel(img->getWidth(), img->getHeight(), CV_32F) {
    cv::Mat aux(img->getWidth(), img->getHeight(), CV_32F);
    std::copy(img->getData().begin(), img->getData().end(), aux.begin<float>());
    cv::flip(aux, m_kernel, -1);
  }

  virtual ~OpenCVConvolution() = default;

  void convolve(std::shared_ptr<VectorImage<SeFloat>> image) const {
    cv::Mat image_cv (image->getHeight(), image->getWidth(), CV_32F);
    std::copy(image->getData().begin(), image->getData().end(), image_cv.begin<float>());

    cv::filter2D(image_cv, image_cv, -1, m_kernel);

    std::copy(image_cv.begin<float>(), image_cv.end<float>(), image->getData().begin());
  }

  std::size_t getWidth() const {
    return m_kernel.size[0];
  }

  std::size_t getHeight() const {
    return m_kernel.size[1];
  }

private:
  cv::Mat m_kernel;// (size, size, CV_32F);
};

} // end of Sextractor

#endif // _SEFRAMEWORK_CONVOLUTION_OPENCVCONVOLUTION_H

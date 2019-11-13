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
/** 
 * @file OpenCvPsf.h
 * @date August 27, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef MODELFITTING_OPENCVPSF_H
#define	MODELFITTING_OPENCVPSF_H

#include <opencv2/opencv.hpp>
#include "ElementsKernel/Exception.h"

namespace ModelFitting {

class OpenCvPsf {
  
public:
  
  OpenCvPsf(double pixel_scale, const cv::Mat& kernel) 
          : m_pixel_scale{pixel_scale}, m_kernel{kernel} {
    if (kernel.cols != kernel.rows) {
      throw Elements::Exception() << "PSF kernel must be square but was "
                                  << kernel.cols << 'X' << kernel.rows;
    }
    if (kernel.cols % 2 == 0) {
      throw Elements::Exception() << "PSF kernel must have odd size, but got "
                                  << kernel.cols;
    }
  }
  
  virtual ~OpenCvPsf() = default;
  
  double getPixelScale() const {
    return m_pixel_scale;
  }
  
  std::size_t getSize() const {
    return m_kernel.cols;
  }
  
//  template <typename ImageType>
//  ImageType getKernel() const;
  
  cv::Mat getKernel() const {
    return m_kernel;
  }

  cv::Mat getScaledKernel(double scale) const {
    return m_kernel * scale;
  }

//  template <typename ImageType>
//  void convolve(ImageType& image) const;
  
  void convolve(cv::Mat& image) const {
    cv::filter2D(image, image, -1, m_kernel);
  }
  
private:
  
  double m_pixel_scale;
  cv::Mat m_kernel;
  
}; // end of class OpenCvPsf

} // endo of ModelFitting

#endif	/* MODELFITTING_OPENCVPSF_H */


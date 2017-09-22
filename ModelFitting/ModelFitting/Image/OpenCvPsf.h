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


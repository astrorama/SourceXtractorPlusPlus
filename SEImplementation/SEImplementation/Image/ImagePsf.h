/*
 * ImagePsf.h
 *
 *  Created on: Sep 19, 2017
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_IMAGE_IMAGEPSF_H_
#define _SEIMPLEMENTATION_IMAGE_IMAGEPSF_H_

#include "ElementsKernel/Exception.h"

#include "SEFramework/Image/VectorImage.h"
#include "SEFramework/Image/MultiplyImage.h"

//#include <opencv2/opencv.hpp>


namespace SExtractor {

class ImagePsf {

public:

  ImagePsf(double pixel_scale, std::shared_ptr<const VectorImage<SExtractor::SeFloat>> image)
          : m_pixel_scale{pixel_scale}, m_image{image} {
    if (image->getWidth() != image->getHeight()) {
      throw Elements::Exception() << "PSF kernel must be square but was "
                                  << image->getWidth() << " x " << image->getHeight();
    }
    if (image->getWidth() % 2 == 0) {
      throw Elements::Exception() << "PSF kernel must have odd size, but got "
                                  << image->getWidth();
    }
  }

  virtual ~ImagePsf() = default;

  double getPixelScale() const {
    return m_pixel_scale;
  }

  std::size_t getSize() const {
    return m_image->getWidth();
  }

  std::shared_ptr<const Image<SExtractor::SeFloat>> getKernel() const {
    return m_image;
  }

  std::shared_ptr<VectorImage<SExtractor::SeFloat>> getScaledKernel(SeFloat scale) const {
    return VectorImage<SeFloat>::create(*MultiplyImage<SExtractor::SeFloat>::create(m_image, scale));
  }

  void convolve(std::shared_ptr<VectorImage<SExtractor::SeFloat>>& image) const {

    throw Elements::Exception() << "This functionality is not available in this branch!";
    // FIXME don't use opencv

    //auto size = m_image->getWidth();
    //cv::Mat kernel (size, size, CV_32F);
    //std::copy(m_image->getData().begin(), m_image->getData().end(), kernel.begin<float>());
    //
    //cv::Mat image_cv (image->getHeight(), image->getWidth(), CV_32F);
    //std::copy(image->getData().begin(), image->getData().end(), image_cv.begin<float>());
    //
    //cv::filter2D(image_cv, image_cv, -1, kernel);
    //
    //std::copy(image_cv.begin<float>(), image_cv.end<float>(), image->getData().begin());
  }

private:

  double m_pixel_scale;
  std::shared_ptr<const VectorImage<SExtractor::SeFloat>> m_image;

};

} // end of ModelFitting



#endif /* _SEIMPLEMENTATION_IMAGE_IMAGEPSF_H_ */

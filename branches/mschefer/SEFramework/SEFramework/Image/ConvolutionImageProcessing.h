/*
 * ConvolutionImageProcessing.h
 *
 *  Created on: Jul 3, 2017
 *      Author: mschefer
 */

#ifndef _SEFRAMEWORK_IMAGE_CONVOLUTIONIMAGEPROCESSING_H_
#define _SEFRAMEWORK_IMAGE_CONVOLUTIONIMAGEPROCESSING_H_

#include <memory>

#include "SEFramework/Image/ImageProcessing.h"

namespace SExtractor {

template<typename T>
class ConvolutionImageProcessing : public ImageProcessing<T> {
public:
  virtual ~ConvolutionImageProcessing() = default;

  explicit ConvolutionImageProcessing(const Image<T>& kernel, bool normalize = false) : m_kernel(kernel) {
  }

  virtual std::shared_ptr<Image<T>> processImage(std::shared_ptr<Image<T>> image) const override {
    auto result_image = std::make_shared<VectorImage<T>>(image->getWidth(), image->getHeight());

    int hx = m_kernel->getWidth() / 2;
    int hy = m_kernel->getHeight() / 2;

    for (int y = 0; y < image->getHeight(); y++) {
      for (int x = 0; x < image->getWidth(); x++) {

        T total = 0;

        for (int cy = 0; cy < m_kernel->getHeight(); cy++) {
          for (int cx = 0; cx < m_kernel->getWidth(); cx++) {

            auto x2 = x + cx - hx;
            auto y2 = y + cy - hy;

            if (x2 >= 0 && x2 < image->getWidth() && y2 >= 0 && y2 < image->getHeight()) {
              total += image->getValue(x2, y2) * m_kernel->getValue(cx, cy);
            }

            result_image->setValue(x, y, total);
          }
        }
      }
    }

    return result_image;

  }

private:
  VectorImage<T> m_kernel;

};


// alternative way to handle borders
//          x2 = std::min(image->getWidth() - 1, std::max(0, x2));
//          y2 = std::min(image->getHeight() - 1, std::max(0, y2));
//
//          auto weight = m_convolution_filter->getValue(cx, cy);
//          total += image->getValue(x2, y2) * weight;
//          total_weight += weight;


}

#endif /* _SEFRAMEWORK_IMAGE_CONVOLUTIONIMAGEPROCESSING_H_ */

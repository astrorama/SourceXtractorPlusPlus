/*
 * ConvolutionImageProcessing.h
 *
 *  Created on: Jul 3, 2017
 *      Author: mschefer
 */

#ifndef _SEFRAMEWORK_IMAGE_CONVOLUTIONIMAGEPROCESSING_H_
#define _SEFRAMEWORK_IMAGE_CONVOLUTIONIMAGEPROCESSING_H_

#include <memory>

#include "SEFramework/Image/BufferedImage.h"
#include "SEFramework/Image/ImageProcessing.h"
#include "SEFramework/Image/ImageSource.h"
#include "SEFramework/Image/ProcessingImageSource.h"

namespace SExtractor {

template<typename T>
class ConvolutionImageProcessing : public ImageProcessing<T> {
private:
  class ConvolutionImageSource : public ProcessingImageSource<T> {
  public:
    ConvolutionImageSource(std::shared_ptr<Image<T>> image, const Image<T>& kernel)
      : ProcessingImageSource<T>(image), m_kernel(kernel) {}

  protected:
    virtual void generateTile(std::shared_ptr<Image<T>> image, ImageTile<T>& tile, int x, int y, int width, int height) const override {
      int hx = m_kernel->getWidth() / 2;
      int hy = m_kernel->getHeight() / 2;

      for (int iy = x; iy < y+height; iy++) {
        for (int ix = y; ix < x+width; ix++) {

          T total = 0;
          T conv_weight=0;

          for (int cy = 0; cy < m_kernel->getHeight(); cy++) {
            for (int cx = 0; cx < m_kernel->getWidth(); cx++) {

              auto x2 = ix + cx - hx;
              auto y2 = iy + cy - hy;

              if (x2 >= 0 && x2 < image->getWidth() && y2 >= 0 && y2 < image->getHeight()) {
                total += image->getValue(x2, y2) * m_kernel->getValue(cx, cy);
                conv_weight += m_kernel->getValue(cx, cy);
              }
            }
          }
          tile->getImage()->setValue(ix - x, iy - y, total / conv_weight);
        }
      }

    }

  private:
    VectorImage<T> m_kernel;

  };

public:
  virtual ~ConvolutionImageProcessing() = default;

  explicit ConvolutionImageProcessing(const Image<T>& kernel, bool normalize = false) : m_kernel(kernel) {
  }

  virtual std::shared_ptr<Image<T>> processImage(std::shared_ptr<Image<T>> image) const override {
    auto result_image = BufferedImage<T>::create(std::make_shared<ConvolutionImageSource>(image, m_kernel));
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

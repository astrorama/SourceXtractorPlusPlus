/*
 * BackgroundConvolution.h
 *
 *  Created on: Oct 12, 2016
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_SEGMENTATION_BACKGROUNDCONVOLUTION_H_
#define _SEIMPLEMENTATION_SEGMENTATION_BACKGROUNDCONVOLUTION_H_

#include "SEFramework/Frame/Frame.h"

#include "SEFramework/Image/VectorImage.h"
#include "SEFramework/Image/ImageProcessing.h"
#include "SEFramework/Image/BufferedImage.h"
#include "SEFramework/Image/ImageSource.h"
#include "SEFramework/Image/ProcessingImageSource.h"

namespace SExtractor {

class BackgroundConvolution : public DetectionImageFrame::ImageFilter {
private:
  class BgConvolutionImageSource : public ProcessingImageSource<DetectionImage::PixelType> {
  public:
    BgConvolutionImageSource(std::shared_ptr<Image<DetectionImage::PixelType>> image,
        std::shared_ptr<DetectionImage> variance, SeFloat threshold,
        std::shared_ptr<VectorImage<SeFloat>> kernel)
      : ProcessingImageSource<DetectionImage::PixelType>(image),
        m_variance(variance), m_threshold(threshold), m_kernel(kernel) {}

  protected:
    virtual void generateTile(std::shared_ptr<Image<DetectionImage::PixelType>> image, ImageTile<DetectionImage::PixelType>& tile, int x, int y, int width, int height) const override {
      int hx = m_kernel->getWidth() / 2;
      int hy = m_kernel->getHeight() / 2;

      for (int iy = y; iy < y+height; iy++) {
        for (int ix = x; ix < x+width; ix++) {

          DetectionImage::PixelType total = 0;
          DetectionImage::PixelType conv_weight = 0;

          for (int cy = 0; cy < m_kernel->getHeight(); cy++) {
            for (int cx = 0; cx < m_kernel->getWidth(); cx++) {

              auto x2 = ix + cx - hx;
              auto y2 = iy + cy - hy;

              if (x2 >= 0 && x2 < image->getWidth() && y2 >= 0 && y2 < image->getHeight() && m_variance->getValue(x2, y2) < m_threshold) {
                total += image->getValue(x2, y2) * m_kernel->getValue(cx, cy);
                conv_weight += m_kernel->getValue(cx, cy);
              }

            }
          }
          tile.getImage()->setValue(ix - x, iy - y, total/conv_weight);
        }
      }

    }

  private:
    std::shared_ptr<DetectionImage> m_variance;
    SeFloat m_threshold;
    std::shared_ptr<VectorImage<SeFloat>> m_kernel;
  };


public:
  BackgroundConvolution(std::shared_ptr<Image<SeFloat>> convolution_filter, bool must_normalize)
    : m_convolution_filter(VectorImage<SeFloat>::create(*convolution_filter))
  {
    if (must_normalize) {
      normalize();
    }
  }

  std::shared_ptr<DetectionImage> processImage(std::shared_ptr<DetectionImage> image, std::shared_ptr<DetectionImage> variance, SeFloat threshold) const;

private:
  void normalize();

  std::shared_ptr<VectorImage<SeFloat>> m_convolution_filter;
};

}

#endif /* _SEIMPLEMENTATION_SEGMENTATION_BACKGROUNDCONVOLUTION_H_ */

/*
 * BackgroundConvolution.h
 *
 *  Created on: Oct 12, 2016
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_SEGMENTATION_BACKGROUNDCONVOLUTION_H_
#define _SEIMPLEMENTATION_SEGMENTATION_BACKGROUNDCONVOLUTION_H_

#include "SEFramework/Frame/Frame.h"
#include "SEFramework/Convolution/DFT.h"
#include "SEFramework/Image/VectorImage.h"
#include "SEFramework/Image/ImageProcessing.h"
#include "SEFramework/Image/BufferedImage.h"
#include "SEFramework/Image/ImageSource.h"
#include "SEFramework/Image/ProcessingImageSource.h"
#include "SEFramework/Image/FunctionalImage.h"
#include <ElementsKernel/Real.h>

namespace SExtractor {

/**
 * Implement an image source using direct convolution of the filter over the image.
 * This approach is normally faster for small kernels
 */
class BgConvolutionImageSource : public ProcessingImageSource<DetectionImage::PixelType> {
public:
  BgConvolutionImageSource(std::shared_ptr<Image<DetectionImage::PixelType>> image,
                           std::shared_ptr<DetectionImage> variance, SeFloat threshold,
                           std::shared_ptr<VectorImage<SeFloat>> kernel)
    : ProcessingImageSource<DetectionImage::PixelType>(image),
      m_variance(variance), m_threshold(threshold) {
    m_kernel = VectorImage<SeFloat>::create(MirrorImage<SeFloat>::create(kernel));
  }

protected:

  std::string getRepr() const override {
    return "BgConvolutionImageSource(" + getImageRepr() + ")";
  }

  virtual void
  generateTile(std::shared_ptr<Image<DetectionImage::PixelType>> image, ImageTile<DetectionImage::PixelType>& tile,
               int x, int y, int width, int height) const override {
    int hx = m_kernel->getWidth() / 2;
    int hy = m_kernel->getHeight() / 2;

    for (int iy = y; iy < y + height; iy++) {
      for (int ix = x; ix < x + width; ix++) {
        DetectionImage::PixelType total = 0;
        DetectionImage::PixelType total_ignoring_threshold = 0;
        DetectionImage::PixelType conv_weight = 0;
        DetectionImage::PixelType conv_weight_ignoring_threshold = 0;

        if (m_variance->getValue(ix, iy) < m_threshold) {

          for (int cy = 0; cy < m_kernel->getHeight(); cy++) {
            for (int cx = 0; cx < m_kernel->getWidth(); cx++) {

              auto x2 = ix + cx - hx;
              auto y2 = iy + cy - hy;

              if (x2 >= 0 && x2 < image->getWidth() && y2 >= 0 && y2 < image->getHeight()) {
                if (m_variance->getValue(x2, y2) < m_threshold) {
                  total += image->getValue(x2, y2) * m_kernel->getValue(cx, cy);
                  conv_weight += m_kernel->getValue(cx, cy);
                }
                total_ignoring_threshold += image->getValue(x2, y2) * m_kernel->getValue(cx, cy);
                conv_weight_ignoring_threshold += m_kernel->getValue(cx, cy);
              }
              if (conv_weight > 0) {
                tile.getImage()->setValue(ix - x, iy - y, total / conv_weight);
              } else {
                tile.getImage()->setValue(ix - x, iy - y, total_ignoring_threshold / conv_weight_ignoring_threshold);
              }
            }
          }
        }
        else {
          tile.getImage()->setValue(ix - x, iy - y, 0);
        }
      }
    }
  }

private:
  std::shared_ptr<DetectionImage> m_variance;
  SeFloat m_threshold;
  std::shared_ptr<VectorImage<SeFloat>> m_kernel;
};

/**
 * Implement an image source using direct convolution of the filter over the image.
 * This approach is normally faster for big kernels
 */
class BgDFTConvolutionImageSource : public ProcessingImageSource<DetectionImage::PixelType> {
public:
  BgDFTConvolutionImageSource(std::shared_ptr<Image<DetectionImage::PixelType>> image,
                              std::shared_ptr<DetectionImage> variance, SeFloat threshold,
                              std::shared_ptr<VectorImage<SeFloat>> kernel)
    : ProcessingImageSource<DetectionImage::PixelType>(image),
      m_variance(variance), m_threshold(threshold), m_convolution(kernel) {
    }

protected:

  // Constant padded
  using PaddedType = PaddedImage<DetectionImage::PixelType>;

  // Discrete Fourier Transform convolution, which is faster for big kernels
  using ConvolutionType = DFTConvolution<DetectionImage::PixelType, PaddedType>;

  std::string getRepr() const override {
    return "BgDFTConvolutionImageSource(" + getImageRepr() + ")";
  }

  virtual void
  generateTile(std::shared_ptr<Image<DetectionImage::PixelType>> image, ImageTile<DetectionImage::PixelType>& tile,
               int start_x, int start_y, int width, int height) const override {
    int hx = m_convolution.getWidth() / 2;
    int hy = m_convolution.getHeight() / 2;
    int clip_x = std::max(start_x - hx, 0);
    int clip_y = std::max(start_y - hy, 0);
    int clip_w = std::min(width + hx * 2, image->getWidth() - clip_x);
    int clip_h = std::min(height + hy * 2, image->getHeight() - clip_y);

    // Clip the image and variance map to the given size, accounting for the margins for the convolution
    auto clipped_img = FunctionalImage<DetectionImage::PixelType>::create(
      clip_w, clip_h, [image, clip_x, clip_y](int x, int y) {
        return image->getValue(clip_x + x, clip_y + y);
      }
    );
    auto clipped_variance = FunctionalImage<DetectionImage::PixelType>::create(
      clip_w, clip_h, [this, clip_x, clip_y](int x, int y) {
        return m_variance->getValue(clip_x + x, clip_y + y);
      }
    );

    // Get the mask
    // For instance, with a threshold of 0.5
    // Variance     Mask       Negative
    // 1  1  1     0  0  0     1  1  1
    // 1  0  1     0  1  0     1  0  1
    // 1  1  1     0  0  0     1  1  1
    auto mask = FunctionalImage<DetectionImage::PixelType>::create(
      clipped_variance->getWidth(), clipped_variance->getHeight(),
      [this, clipped_variance](int x, int y) -> DetectionImage::PixelType {
        return clipped_variance->getValue(x, y) < m_threshold;
      }
    );

    // Get the image masking out values where the variance is greater than the threshold
    auto masked_img = MultiplyImage<DetectionImage::PixelType>::create(clipped_img, mask);

    // Convolve the masked image, padding with 0
    auto conv_masked = VectorImage<DetectionImage::PixelType>::create(masked_img);
    m_convolution.convolve(conv_masked);

    // Convolve the mask
    // This gives us in each cell the sum of the kernel values that have been used,
    // so we can divide the previous convolution.
    auto conv_mask = VectorImage<DetectionImage::PixelType>::create(mask);
    m_convolution.convolve(conv_mask);

    // Copy out the value of the convolved image, divided by the negative mask, applying
    // again the mask to the convolved result
    int off_x = start_x - clip_x;
    int off_y = start_y - clip_y;
    for (int y = 0; y < height; ++y) {
      for (int x = 0; x < width; ++x) {
        if (mask->getValue(x + off_x, y + off_y)) {
          tile.setValue(
            x + start_x, y + start_y,
            conv_masked->getValue(x + off_x, y + off_y) / conv_mask->getValue(x + off_x, y + off_y)
          );
        } else {
          tile.setValue(x + start_x, y + start_y, 0);
        }
      }
    }
  }

private:
  std::shared_ptr<DetectionImage> m_variance;
  DetectionImage::PixelType m_threshold;
  ConvolutionType m_convolution;
};

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

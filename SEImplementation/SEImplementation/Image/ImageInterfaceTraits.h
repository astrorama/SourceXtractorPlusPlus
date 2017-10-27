/*
 * ImageInterfaceTraits.h
 *
 *  Created on: May 3, 2017
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_IMAGE_IMAGEINTERFACETRAITS_H_
#define _SEIMPLEMENTATION_IMAGE_IMAGEINTERFACETRAITS_H_

#define INTERP_MAXKERNELWIDTH 8 // Max. range of kernel (pixels)

#include <boost/math/constants/constants.hpp>
#include <cmath>
#include <algorithm>
#include <memory>
#include <utility>
#include <vector>

#include "ModelFitting/Image/ImageTraits.h"
#include "SEFramework/Image/Image.h"
#include "SEFramework/Image/VectorImage.h"

namespace ModelFitting {

// Interpolation types
typedef enum  {INTERP_NEARESTNEIGHBOUR, INTERP_BILINEAR, INTERP_LANCZOS2,
    INTERP_LANCZOS3, INTERP_LANCZOS4}       interpenum;

using ImageInterfaceType = SExtractor::VectorImage<SExtractor::SeFloat>;
using ImageInterfaceTypePtr = std::shared_ptr<ImageInterfaceType>;

template <>
struct ImageTraits<ImageInterfaceTypePtr> {

  using iterator = std::vector<ImageInterfaceType::PixelType>::iterator;

  static ImageInterfaceTypePtr factory(std::size_t width, std::size_t height) {
    return SExtractor::VectorImage<ImageInterfaceType::PixelType>::create(width, height);
  }

  static std::size_t width(const ImageInterfaceTypePtr& image) {
    return image->getWidth();
  }

  static std::size_t height(const ImageInterfaceTypePtr& image) {
    return image->getHeight();
  }

  static ImageInterfaceType::PixelType& at(ImageInterfaceTypePtr& image, std::size_t x, std::size_t y) {
    return image->at(x, y);
  }

  static ImageInterfaceType::PixelType at(const ImageInterfaceTypePtr& image, std::size_t x, std::size_t y) {
    return image->at(x, y);
  }

  static iterator begin(const ImageInterfaceTypePtr& image) {
    return image->getData().begin();
  }

  static iterator end(const ImageInterfaceTypePtr& image) {
    return image->getData().end();
  }

  static void addImageToImage(ImageInterfaceTypePtr& image1, const ImageInterfaceTypePtr& image2,
                              double scale_factor, double x, double y) {
    // Calculate the size in pixels of the image2 after in the scale of image1
    double scaled_width = width(image2) * scale_factor;
    double scaled_height = height(image2) * scale_factor;
    // Calculate the window of the image1 which is affected
    int x_min = std::floor(x - scaled_width / 2.);
    int x_max = std::ceil(x + scaled_width / 2.);
    int window_width = x_max - x_min;
    int y_min = std::floor(y - scaled_height / 2.);
    int y_max= std::ceil(y + scaled_height / 2.);
    int window_height = y_max - y_min;
    // Calculate the shift of the image2 inside the window
    double x_shift = x - scaled_width / 2. - x_min;
    double y_shift = y - scaled_height / 2. - y_min;
    // Create the scaled and shifted window
    auto window = factory(window_width, window_height);

    shiftResizeLancszos(image2, window, scale_factor, x_shift, y_shift);

    // We need to correct the window for the scaling, so it has the same integral
    // with the image2
    double corr_factor = 1. / (scale_factor * scale_factor);
    // Add the window to the image1
    for(int x_im=std::max(x_min,0); x_im<std::min<int>(x_max, width(image1)); ++x_im) {
      for (int y_im=std::max(y_min,0); y_im<std::min<int>(y_max, height(image1)); ++y_im) {
        int x_win = x_im - x_min;
        int y_win = y_im - y_min;
        at(image1, x_im, y_im) += corr_factor * at(window, x_win, y_win);
      }
    }
  }

  static double getClamped(const ImageInterfaceTypePtr& image, int x, int y) {
    return at(image, std::max(0, std::min(x, (int) width(image) - 1)), std::max(0, std::min(y, (int) height(image) - 1)));
  }

  static void shiftResize(const ImageInterfaceTypePtr& source, ImageInterfaceTypePtr& window, double scale_factor, double x_shift, double y_shift) {
    int window_width = width(window);
    int window_height = height(window);
    for(int x_win=0; x_win < window_width; x_win++) {
      for(int y_win=0; y_win < window_height; y_win++) {
        double x = (x_win - 0.5 - x_shift) / scale_factor;
        double y = (y_win - 0.5 - y_shift) / scale_factor;

        int xi = std::floor(x);
        int yi = std::floor(y);

        double x_delta = x - xi;
        double y_delta = y - yi;

        double v00 = getClamped(source, xi, yi);
        double v01 = getClamped(source, xi, yi+1);
        double v10 = getClamped(source, xi+1, yi);
        double v11 = getClamped(source, xi+1, yi+1);

        at(window, x_win, y_win) = (1.0 - y_delta) * ((1.0 - x_delta) * v00 + x_delta * v10) +
                                           y_delta * ((1.0 - x_delta) * v01 + x_delta * v11);
      }
    }

  }

  static void shiftResizeLancszos(const ImageInterfaceTypePtr& source, ImageInterfaceTypePtr& window, double scale_factor, double x_shift, double y_shift) {
    int window_width = width(window);
    int window_height = height(window);
    for(int x_win=0; x_win < window_width; x_win++) {
      for(int y_win=0; y_win < window_height; y_win++) {
        float x = (x_win - x_shift) / scale_factor;
        float y = (y_win - y_shift) / scale_factor;

        at(window, x_win, y_win) = interpolate_pix(&source->getData()[0], x, y, source->getWidth(), source->getHeight(), INTERP_LANCZOS4);
      }
    }

  }

  static float interpolate_pix(float *pix, float x, float y,
      int xsize, int ysize, interpenum interptype) {

     static const int interp_kernwidth[5]={1,2,4,6,8};

     float    buffer[INTERP_MAXKERNELWIDTH],
      kernel[INTERP_MAXKERNELWIDTH],
      *kvector, *pixin, *pixout,
      dx, dy, val;
     int    i, j, ix, iy, kwidth, step;

    kwidth = interp_kernwidth[interptype];

//-- Get the integer part of the current coordinate or nearest neighbour
    if (interptype == INTERP_NEARESTNEIGHBOUR) {
      ix = (int)(x-0.50001);
      iy = (int)(y-0.50001);
    } else {
      ix = (int)x;
      iy = (int)y;
    }

//-- Store the fractional part of the current coordinate
    dx = x - ix;
    dy = y - iy;
//-- Check if interpolation start/end exceed image boundary
    ix -= kwidth / 2;
    iy -= kwidth / 2;
    if (ix < 0 || ix + kwidth <= 0 || ix + kwidth > xsize ||
  iy < 0 || iy + kwidth <= 0 || iy + kwidth > ysize)
      return 0.0;

//-- First step: interpolate along NAXIS1 from the data themselves
    make_kernel(dx, kernel, interptype);
    step = xsize - kwidth;
    pixin = pix + iy * xsize + ix ; // Set starting pointer
    pixout = buffer;
    for (j = kwidth; j--;) {
      val = 0.0;
      kvector = kernel;
      for (i = kwidth; i--;)
        val += *(kvector++) * *(pixin++);
      *(pixout++) = val;
      pixin += step;
    }

//-- Second step: interpolate along NAXIS2 from the interpolation buffer
    make_kernel(dy, kernel, interptype);
    pixin = buffer;
    val = 0.0;
    kvector = kernel;
    for (i = kwidth; i--;)
      val += *(kvector++) * *(pixin++);

    return val;
  }


  static void make_kernel(float pos, float *kernel, interpenum interptype) {
   const float pi = boost::math::constants::pi<float>();
   const float threshold = 1e-6;
   float  x, val, sinx1,sinx2,sinx3,cosx1;

    if (interptype == INTERP_NEARESTNEIGHBOUR)
      *kernel = 1.0;
    else if (interptype == INTERP_BILINEAR) {
      *(kernel++) = 1.0 - pos;
      *kernel = pos;
    } else if (interptype == INTERP_LANCZOS2) {
      if (pos < threshold && pos > - threshold) {
        *(kernel++) = 0.0;
        *(kernel++) = 1.0;
        *(kernel++) = 0.0;
        *kernel = 0.0;
      } else {
        x = - pi / 2.0 * (pos + 1.0);
        sincosf(x, &sinx1, &cosx1);
        val = (*(kernel++) = sinx1 / (x * x));
        x += pi / 2.0;
        val += (*(kernel++) = -cosx1 / (x * x));
        x += pi / 2.0;
        val += (*(kernel++) = -sinx1 / (x * x));
        x += pi / 2.0;
        val += (*kernel = cosx1 / (x * x));
        val = 1.0/val;
        *(kernel--) *= val;
        *(kernel--) *= val;
        *(kernel--) *= val;
        *kernel *= val;
      }
    } else if (interptype == INTERP_LANCZOS3) {
      if (pos < threshold && pos > - threshold) {
        *(kernel++) = 0.0;
        *(kernel++) = 0.0;
        *(kernel++) = 1.0;
        *(kernel++) = 0.0;
        *(kernel++) = 0.0;
        *kernel = 0.0;
      } else {
        x = - pi / 3.0 * (pos + 2.0);
        sincosf(x, &sinx1, &cosx1);
        val = (*(kernel++) = sinx1 / (x * x));
        x += pi / 3.0;
        val += (*(kernel++) = (sinx2 = -0.5 * sinx1 - 0.866025403785 * cosx1)
        / (x*x));
        x += pi / 3.0;
        val += (*(kernel++) = (sinx3 = - 0.5 * sinx1 + 0.866025403785 * cosx1)
        / (x * x));
        x += pi / 3.0;
        val += (*(kernel++) = sinx1 / (x * x));
        x += pi / 3.0;
        val += (*(kernel++) = sinx2 / (x * x));
        x += pi / 3.0;
        val += (*kernel = sinx3 / (x * x));
        val = 1.0 / val;
        *(kernel--) *= val;
        *(kernel--) *= val;
        *(kernel--) *= val;
        *(kernel--) *= val;
        *(kernel--) *= val;
        *kernel *= val;
      }
    } else if (interptype == INTERP_LANCZOS4) {
      if (pos < threshold && pos > - threshold) {
        *(kernel++) = 0.0;
        *(kernel++) = 0.0;
        *(kernel++) = 0.0;
        *(kernel++) = 1.0;
        *(kernel++) = 0.0;
        *(kernel++) = 0.0;
        *(kernel++) = 0.0;
        *kernel = 0.0;
      } else {
        x = - pi / 4.0 * (pos + 3.0);
        sincosf(x, &sinx1, &cosx1);
        val = (*(kernel++) = sinx1 / (x * x));
        x += pi / 4.0;
        val += (*(kernel++) = - (sinx2 = 0.707106781186 * (sinx1 + cosx1))
        / (x * x));
        x += pi / 4.0;
        val += (*(kernel++) = cosx1 / (x * x));
        x += pi / 4.0;
        val += (*(kernel++) = - (sinx3 = 0.707106781186 * (cosx1 - sinx1))
        /(x * x));
        x += pi / 4.0;
        val += (*(kernel++) = -sinx1 / (x * x));
        x += pi / 4.0;
        val += (*(kernel++) = sinx2 / (x * x));
        x += pi / 4.0;
        val += (*(kernel++) = -cosx1 / (x * x));
        x += pi / 4.0;
        val += (*kernel = sinx3 / (x * x));
        val = 1.0 / val;
        *(kernel--) *= val;
        *(kernel--) *= val;
        *(kernel--) *= val;
        *(kernel--) *= val;
        *(kernel--) *= val;
        *(kernel--) *= val;
        *(kernel--) *= val;
        *kernel *= val;
      }
    }

  }

}; // end of class ImageTraits<ImageInterfaceTypePtr>

} // end of namespace ModelFitting


#endif /* _SEIMPLEMENTATION_IMAGE_IMAGEINTERFACETRAITS_H_ */

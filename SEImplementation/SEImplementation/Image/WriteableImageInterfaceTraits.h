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
 * WriteableImageInterfaceTraits.h
 *
 *  Created on: Jan 15, 2019
 *      Author: Alejandro Alvarez Ayllon
 */

#ifndef _SEIMPLEMENTATION_IMAGE_WRITEABLEIMAGEINTERFACETRAITS_H_
#define _SEIMPLEMENTATION_IMAGE_WRITEABLEIMAGEINTERFACETRAITS_H_

#include "SEFramework/Image/ImageAccessor.h"
#include "ImageInterfaceTraits.h"

namespace ModelFitting {

using WriteableInterfaceType = SourceXtractor::WriteableImage<SourceXtractor::SeFloat>;
using WriteableInterfaceTypePtr = std::shared_ptr<WriteableInterfaceType>;


/**
 * Specialization of the ImageTraits for WriteableImage
 * Can be used for WriteableBufferImages, for instance.
 * @warning This specialization is very inefficient, but handy for generating test images
 */
template<>
struct ImageTraits<WriteableInterfaceTypePtr> {

  class WriteableIterator;

  class WriteableSetter {
  private:
    WriteableInterfaceTypePtr m_image;
    SourceXtractor::ImageAccessor<WriteableInterfaceType::PixelType> m_accessor;
    int m_x, m_y;

    friend class WriteableIterator;

  public:
    WriteableSetter(WriteableInterfaceTypePtr &image, int x, int y)
    : m_image{image}, m_accessor{image}, m_x{x}, m_y{y} {}

    WriteableInterfaceType::PixelType operator=(WriteableInterfaceType::PixelType v) {
      m_image->setValue(m_x, m_y, v);
      return v;
    }

    WriteableInterfaceType::PixelType operator+=(WriteableInterfaceType::PixelType v) {
      v += m_accessor.getValue(m_x, m_y);
      m_image->setValue(m_x, m_y, v);
      return v;
    }

    operator WriteableInterfaceType::PixelType () {
      return m_accessor.getValue(m_x, m_y);
    }
  };

  class WriteableIterator : std::iterator<std::forward_iterator_tag, WriteableInterfaceType::PixelType> {
  private:
    WriteableInterfaceTypePtr m_image;
    SourceXtractor::ImageAccessor<WriteableInterfaceType::PixelType> m_accessor;
    int m_x, m_y;
    const int m_width, m_height;
    WriteableSetter m_setter;

  public:

    WriteableIterator(WriteableInterfaceTypePtr image)
      : m_image{image}, m_accessor{image}, m_x{0}, m_y{0},
        m_width{image->getWidth()},
        m_height{image->getHeight()},
        m_setter{image, 0, 0} {
    }

    WriteableIterator(WriteableInterfaceTypePtr image, int) : WriteableIterator(image) {
      m_y = m_height;
    }

    bool operator!=(const WriteableIterator &b) const {
      return m_x != b.m_x || m_y != b.m_y || m_width != b.m_width || m_height != b.m_height || m_image != b.m_image;
    }

    WriteableIterator &operator++() {
      if (m_y < m_height) {
        ++m_x;
        if (m_x >= m_width) {
          m_x = 0;
          ++m_y;
          if (m_y >= m_height) {
            m_y = m_height;
          }
        }
      }
      return *this;
    }
/*
    WriteableInterfaceType::PixelType operator*() {
      return m_accessor.getValue(m_x, m_y);
    }
*/
    WriteableSetter& operator*() {
      m_setter.m_x = m_x;
      m_setter.m_y = m_y;
      return m_setter;
    }
  };

  using iterator = WriteableIterator;

  static WriteableInterfaceTypePtr factory(std::size_t width, std::size_t height) {
    return SourceXtractor::VectorImage<ImageInterfaceType::PixelType>::create(width, height);
  }

  static std::size_t width(const WriteableInterfaceTypePtr &image) {
    return image->getWidth();
  }

  static std::size_t height(const WriteableInterfaceTypePtr &image) {
    return image->getHeight();
  }

  static WriteableSetter at(WriteableInterfaceTypePtr &image, std::size_t x, std::size_t y) {
    return WriteableSetter(image, x, y);
  }

  static ImageInterfaceType::PixelType at(const WriteableInterfaceTypePtr &image, std::size_t x, std::size_t y) {
    //return image->getValue(x, y);
    abort();
  }

  static iterator begin(const WriteableInterfaceTypePtr &image) {
    return WriteableIterator{image};
  }

  static iterator end(const WriteableInterfaceTypePtr &image) {
    return WriteableIterator{image, 0};
  }

  static void addImageToImage(WriteableInterfaceTypePtr &image1, const WriteableInterfaceTypePtr &image2,
                              double scale_factor, double x, double y);

  static double getClamped(const WriteableInterfaceTypePtr &image, int x, int y) {
    return at(image, std::max(0, std::min(x, (int) width(image) - 1)),
              std::max(0, std::min(y, (int) height(image) - 1)));
  }

  static void
  shiftResize(const WriteableInterfaceTypePtr &source, WriteableInterfaceTypePtr &window, double scale_factor,
              double x_shift, double y_shift) {
    int window_width = width(window);
    int window_height = height(window);
    for (int x_win = 0; x_win < window_width; x_win++) {
      for (int y_win = 0; y_win < window_height; y_win++) {
        double x = (x_win - 0.5 - x_shift) / scale_factor;
        double y = (y_win - 0.5 - y_shift) / scale_factor;

        int xi = std::floor(x);
        int yi = std::floor(y);

        double x_delta = x - xi;
        double y_delta = y - yi;

        double v00 = getClamped(source, xi, yi);
        double v01 = getClamped(source, xi, yi + 1);
        double v10 = getClamped(source, xi + 1, yi);
        double v11 = getClamped(source, xi + 1, yi + 1);

        window->setValue(x_win, y_win, (1.0 - y_delta) * ((1.0 - x_delta) * v00 + x_delta * v10) +
                                       y_delta * ((1.0 - x_delta) * v01 + x_delta * v11));
      }
    }

  }

  static void
  shiftResizeLancszos(const WriteableInterfaceTypePtr &source, WriteableInterfaceTypePtr &window, double scale_factor,
                      double x_shift, double y_shift) {
    SourceXtractor::ImageAccessor<WriteableInterfaceType::PixelType> accessor(source);
    int window_width = width(window);
    int window_height = height(window);
    for (int x_win = 0; x_win < window_width; x_win++) {
      for (int y_win = 0; y_win < window_height; y_win++) {
        float x = (x_win - x_shift) / scale_factor;
        float y = (y_win - y_shift) / scale_factor;

        // Work around the interface of interpolate_pix copying to memory the pixels surrounding
        // the pixel to interpolate
        float buffer[INTERP_MAXKERNELWIDTH * INTERP_MAXKERNELWIDTH] = {0.};
        int ix = x, iy = y;

        for (int j = 0; j < INTERP_MAXKERNELWIDTH; ++j) {
          for (int i = 0; i < INTERP_MAXKERNELWIDTH; ++i) {
            int src_x = ix + i - INTERP_MAXKERNELWIDTH / 2;
            int src_y = iy + j - INTERP_MAXKERNELWIDTH / 2;

            if (source->isInside(src_x, src_y)) {
              buffer[j * INTERP_MAXKERNELWIDTH + i] = accessor.getValue(src_x, src_y);
            }
          }
        }

        window->setValue(x_win, y_win,
                         SourceXtractor::interpolate_pix(buffer, INTERP_MAXKERNELWIDTH / 2 + x - ix,
                                                         INTERP_MAXKERNELWIDTH / 2 + y - iy,
                                                         8, 8, SourceXtractor::INTERP_LANCZOS4));
      }
    }

  }


}; // end of class ImageTraits<WriteableInterfaceTypePtr>
/*
template<typename T>
auto operator*(T v, const ImageTraits<WriteableInterfaceTypePtr>::WriteableSetter &setter) -> decltype(v *
                                                                                                       WriteableInterfaceType::PixelType(
                                                                                                         0)) {
  return v * static_cast<WriteableInterfaceType::PixelType>(setter);
}

template<typename T>
auto operator*(const ImageTraits<WriteableInterfaceTypePtr>::WriteableSetter &setter, T v) -> decltype(
WriteableInterfaceType::PixelType(0) * v) {
  return static_cast<WriteableInterfaceType::PixelType>(setter) * v;
}
*/
inline void ImageTraits<WriteableInterfaceTypePtr>::addImageToImage(WriteableInterfaceTypePtr &image1,
                                                                    const WriteableInterfaceTypePtr &image2,
                                                                    double scale_factor, double x, double y) {
  // Calculate the size in pixels of the image2 after in the scale of image1
  double scaled_width = width(image2) * scale_factor;
  double scaled_height = height(image2) * scale_factor;
  // Calculate the window of the image1 which is affected
  int x_min = std::floor(x - scaled_width / 2.);
  int x_max = std::ceil(x + scaled_width / 2.);
  int window_width = x_max - x_min;
  int y_min = std::floor(y - scaled_height / 2.);
  int y_max = std::ceil(y + scaled_height / 2.);
  int window_height = y_max - y_min;
  // Calculate the shift of the image2 inside the window
  double x_shift = x - scaled_width / 2. - x_min;
  double y_shift = y - scaled_height / 2. - y_min;
  // Create the scaled and shifted window
  auto window = factory(window_width, window_height);

  //shiftResize(image2, window, scale_factor, x_shift, y_shift);
  shiftResizeLancszos(image2, window, scale_factor, x_shift, y_shift);

  // We need to correct the window for the scaling, so it has the same integral
  // with the image2
  double corr_factor = 1. / (scale_factor * scale_factor);
  // Add the window to the image1
  for (int x_im = std::max(x_min, 0); x_im < std::min<int>(x_max, width(image1)); ++x_im) {
    for (int y_im = std::max(y_min, 0); y_im < std::min<int>(y_max, height(image1)); ++y_im) {
      int x_win = x_im - x_min;
      int y_win = y_im - y_min;
      at(image1, x_im, y_im) += corr_factor * at(window, x_win, y_win);
    }
  }
}

} // end of namespace ModelFitting


#endif /* _SEIMPLEMENTATION_IMAGE_WRITEABLEIMAGEINTERFACETRAITS_H_ */

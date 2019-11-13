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
 * InterpolatedImage.h
 *
 *  Created on: Apr 18, 2018
 *      Author: mschefer
 */

#ifndef SEFRAMEWORK_SEFRAMEWORK_IMAGE_INTERPOLATEDIMAGE_H_
#define SEFRAMEWORK_SEFRAMEWORK_IMAGE_INTERPOLATEDIMAGE_H_

#include "SEFramework/Image/ImageBase.h"

namespace SExtractor {

template <typename T>
class InterpolatedImage : public ImageBase<T> {
protected:

  InterpolatedImage(std::shared_ptr<Image<T>> image, std::shared_ptr<Image<T>> variance_map, T variance_threshold, int max_gap)
      : m_image(image), m_variance_map(variance_map), m_variance_threshold(variance_threshold), m_max_gap(max_gap) {}

public:
  virtual ~InterpolatedImage() = default;

  static std::shared_ptr<InterpolatedImage<T>> create(std::shared_ptr<Image<T>> image, std::shared_ptr<Image<T>> variance_map, T variance_threshold, int max_gap) {
    return std::shared_ptr<InterpolatedImage<T>>(new InterpolatedImage<T>(image, variance_map, variance_threshold, max_gap));
  }

  virtual std::string getRepr() const override {
    return "InterpolatedImage(" + m_image->getRepr() + "," + m_variance_map->getRepr() + ")";
  }

  /// Returns the value of the pixel with the coordinates (x,y)
  virtual T getValue(int x, int y) const override {
    if (isPixelGood(x, y)) {
      return m_image->getValue(x, y);
    }

    for (int i=1; i <= m_max_gap; i++) {
      if (x-i >= 0 && isPixelGood(x-i, y)) {
        return m_image->getValue(x-i, y);
      }
      if (y-i >= 0 && isPixelGood(x, y-i)) {
        return m_image->getValue(x, y-i);
      }
    }

    // Couldn't interpolate, return what we have
    return m_image->getValue(x, y);
  }

  virtual int getWidth() const override {
    return m_image->getWidth();
  }

  virtual int getHeight() const override {
    return m_image->getHeight();
  }

private:
  bool isPixelGood(int x, int y) const {
    return m_variance_map->getValue(x, y) < m_variance_threshold;
  }

  std::shared_ptr<Image<T>> m_image;
  std::shared_ptr<Image<T>> m_variance_map;
  T m_variance_threshold;

  int m_max_gap;
};


}


#endif /* SEFRAMEWORK_SEFRAMEWORK_IMAGE_INTERPOLATEDIMAGE_H_ */

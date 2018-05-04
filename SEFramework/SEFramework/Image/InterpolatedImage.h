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

  InterpolatedImage(std::shared_ptr<Image<T>> image, std::shared_ptr<Image<T>> variance_map, T variance_threshold)
      : m_image(image), m_variance_map(variance_map), m_variance_threshold(variance_threshold) {}

public:
  virtual ~InterpolatedImage() = default;

  static std::shared_ptr<InterpolatedImage<T>> create(std::shared_ptr<Image<T>> image, std::shared_ptr<Image<T>> variance_map, T variance_threshold) {
    return std::shared_ptr<InterpolatedImage<T>>(new InterpolatedImage<T>(image, variance_map, variance_threshold));
  }

  /// Returns the value of the pixel with the coordinates (x,y)
  virtual T getValue(int x, int y) const override {
    if (isPixelGood(x, y)) {
      return m_image->getValue(x, y);
    }

    return x > 0 ? getValue(x-1, y) : 0; // FIXME !!!!! test only
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
};


}




#endif /* SEFRAMEWORK_SEFRAMEWORK_IMAGE_INTERPOLATEDIMAGE_H_ */

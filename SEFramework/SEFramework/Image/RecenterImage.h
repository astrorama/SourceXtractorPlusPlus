/*
 * @file SEFramework/Image/WarpImage.h
 * @date 11/09/18
 * @author Alejandro Alvarez Ayllon
 */

#ifndef _SEFRAMEWORK_IMAGE_WARPIMAGE_H
#define _SEFRAMEWORK_IMAGE_WARPIMAGE_H

#include "SEFramework/Image/ImageBase.h"

namespace SExtractor {

/**
 * @class RecenterImage
 * @brief Changes the center of an image, wrapping it around the edges
 */
template<typename T>
class RecenterImage : public ImageBase<T> {
protected:
  RecenterImage(std::shared_ptr<const Image<T>> img, const PixelCoordinate &new_center) : m_img{img},
                                                                                          m_center{new_center} {
  }

public:
  template<typename... Args>
  static std::shared_ptr<RecenterImage<T>> create(Args &&... args) {
    return std::shared_ptr<RecenterImage<T>>(new RecenterImage{std::forward<Args>(args)...});
  }

  int getWidth() const override {
    return m_img->getWidth();
  }

  int getHeight() const override {
    return m_img->getHeight();
  }

  T getValue(int x, int y) const override {
    x = (x + m_center.m_x) % m_img->getWidth();
    y = (y + m_center.m_y) % m_img->getHeight();
    return m_img->getValue(x, y);
  }

private:
  std::shared_ptr<const Image<T>> m_img;
  PixelCoordinate m_center;
};

} // end SExtractor

#endif // _SEFRAMEWORK_IMAGE_WARPIMAGE_H

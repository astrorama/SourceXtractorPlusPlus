/*
 * @file SEFramework/Image/ReflectImage.h
 * @date 11/09/18
 * @author Alejandro Alvarez Ayllon
 */

#ifndef _SEFRAMEWORK_IMAGE_MIRRORIMAGE_H
#define _SEFRAMEWORK_IMAGE_MIRRORIMAGE_H

#include "SEFramework/Image/ImageBase.h"

namespace SExtractor {

/**
 * @class MirrorImage
 * @brief Mirrors an image in both X and Y axes
 */
template <typename T>
class MirrorImage: public ImageBase<T> {
protected:
  MirrorImage(std::shared_ptr<const Image<T>> img) : m_img{img} {
  }

public:
  template<typename... Args>
  static std::shared_ptr<MirrorImage<T>> create(Args &&... args) {
    return std::shared_ptr<MirrorImage<T>>(new MirrorImage{std::forward<Args>(args)...});
  }

  std::string getRepr() const override {
    return "MirrorImage(" + m_img->getRepr() + ")";
  }

  int getWidth() const override {
    return m_img->getWidth();
  }

  int getHeight() const override {
    return m_img->getHeight();
  }

  T getValue(int x, int y) const override {
    x = m_img->getWidth() - x - 1;
    y = m_img->getHeight() - y - 1;
    return m_img->getValue(x, y);
  }

private:
  std::shared_ptr<const Image<T>> m_img;
};

} // end SExtractor

#endif // _SEFRAMEWORK_IMAGE_MIRRORIMAGE_H

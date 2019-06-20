/*
 * @file SEFramework/Image/FunctionalImage.h
 * @date 27/03/19
 * @author Alejandro Alvarez Ayllon
 */

#ifndef _SEFRAMEWORK_IMAGE_FUNCTIONALIMAGE_H
#define _SEFRAMEWORK_IMAGE_FUNCTIONALIMAGE_H

#include "SEFramework/Image/ImageBase.h"

namespace SExtractor {

template<typename T>
class FunctionalImage : public ImageBase<T> {
public:
  using FunctorType = std::function<T(int x, int y)>;

protected:
  FunctionalImage(int width, int height, FunctorType functor)
    : m_width{width}, m_height{height}, m_functor{functor} {
  }

public:
  virtual ~FunctionalImage() = default;

  template<typename ...Args>
  static std::shared_ptr<ImageBase<T>> create(Args&& ... args) {
    return std::shared_ptr<FunctionalImage<T>>(new FunctionalImage{std::forward<Args>(args)...});
  }

  std::string getRepr() const override {
    return "FunctionalImage<" + std::string(m_functor.target_type().name()) + ">";
  }

  T getValue(int x, int y) const override {
    return m_functor(x, y);
  }

  int getWidth() const override {
    return m_width;
  }

  int getHeight() const override {
    return m_height;
  }

private:
  int m_width, m_height;
  FunctorType m_functor;
};

} // end SExtractor

#endif // _SEFRAMEWORK_IMAGE_FUNCTIONALIMAGE_H

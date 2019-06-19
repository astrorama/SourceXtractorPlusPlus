/*
 * ConstantImage.h
 *
 *  Created on: Jan 12, 2017
 *      Author: mschefer
 */

#ifndef _SEFRAMEWORK_IMAGE_CONSTANTIMAGE_H_
#define _SEFRAMEWORK_IMAGE_CONSTANTIMAGE_H_

#include "SEFramework/Image/Image.h"
#include "SEFramework/Image/ImageBase.h"

namespace SExtractor {

template <typename T>
class ConstantImage : public ImageBase<T> {
protected:

  ConstantImage(int width, int height, T constant_value)
      : m_width(width), m_height(height), m_constant_value(constant_value) {}


public:

  static std::shared_ptr<ConstantImage<T>> create(int width, int height, T constant_value) {
    return std::shared_ptr<ConstantImage<T>>(new ConstantImage(width, height, constant_value));
  }

  virtual ~ConstantImage() = default;

  std::string getRepr() const override {
    return "ConstantImage<" + std::to_string(m_width) + "," + std::to_string(m_height) + ">(" +
           std::to_string(m_constant_value) + ")";
  }

  int getHeight() const override {
    return m_height;
  }

  int getWidth() const override {
    return m_width;
  }

  using Image<T>::getValue;
  T getValue(int, int) const override {
    return m_constant_value;
  }

private:

  int m_width;
  int m_height;
  T m_constant_value;

};

}


#endif /* _SEFRAMEWORK_IMAGE_CONSTANTIMAGE_H_ */

/*
 * ConstantImage.h
 *
 *  Created on: Jan 12, 2017
 *      Author: mschefer
 */

#ifndef _SEFRAMEWORK_IMAGE_CONSTANTIMAGE_H_
#define _SEFRAMEWORK_IMAGE_CONSTANTIMAGE_H_

#include "SEFramework/Image/Image.h"

namespace SExtractor {

template <typename T>
class ConstantImage : public Image<T> {
public:

  ConstantImage(int width, int height, T constant_value)
      : m_width(width), m_height(height), m_constant_value(constant_value) {}

  virtual ~ConstantImage() = default;

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

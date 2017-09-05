/*
 * MultiplyImage.h
 *
 *  Created on: Apr 25, 2017
 *      Author: mschefer
 */

#ifndef _SEFRAMEWORK_IMAGE_MULTIPLYIMAGE_H_
#define _SEFRAMEWORK_IMAGE_MULTIPLYIMAGE_H_

#include <memory>

#include "SEFramework/Image/Image.h"
#include "SEFramework/Image/ImageBase.h"
#include "SEFramework/Image/ConstantImage.h"

namespace SExtractor {

template <typename T>
class MultiplyImage : public ImageBase<T> {

public:

  /**
   * @brief Destructor
   */
  virtual ~MultiplyImage() = default;

  MultiplyImage(std::shared_ptr<Image<T>> image, T value_to_multiply)
      : m_image(image), m_image_to_multiply(
          std::make_shared<ConstantImage<T>>(image->getWidth(), image->getHeight(), value_to_multiply)) {};

  MultiplyImage(std::shared_ptr<Image<T>> image, std::shared_ptr<Image<T>> image_to_multiply)
      : m_image(image), m_image_to_multiply(image_to_multiply) {
    assert(m_image->getWidth() == m_image_to_multiply->getWidth());
    assert(m_image->getHeight() == m_image_to_multiply->getHeight());
  };

  using Image<T>::getValue;
  T getValue(int x, int y) const override {
    return m_image->getValue(x, y) * m_image_to_multiply->getValue(x, y);
  }

  int getWidth() const override {
    return m_image->getWidth();
  }

  int getHeight() const override {
    return m_image->getHeight();
  }

private:
  std::shared_ptr<Image<T>> m_image, m_image_to_multiply;

};

} /* namespace SExtractor */



#endif /* _SEFRAMEWORK_IMAGE_MULTIPLYIMAGE_H_ */

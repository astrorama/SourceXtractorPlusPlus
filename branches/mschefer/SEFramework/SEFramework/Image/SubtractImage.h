/**
 * @file SEFramework/Image/SubtractImage.h
 * @date 06/02/16
 * @author mschefer
 */

#ifndef _SEFRAMEWORK_IMAGE_SUBTRACTIMAGE_H
#define _SEFRAMEWORK_IMAGE_SUBTRACTIMAGE_H

#include <memory>

#include "SEFramework/Image/Image.h"
#include "SEFramework/Image/ImageBase.h"
#include "SEFramework/Image/ConstantImage.h"

namespace SExtractor {

/**
 * @class SubtractImage
 * @brief Used to subtract a constant value from an Image
 *
 */
template <typename T>
class SubtractImage : public ImageBase<T> {

public:

  /**
   * @brief Destructor
   */
  virtual ~SubtractImage() = default;

  SubtractImage(std::shared_ptr<Image<T>> image, T value_to_subtract)
      : m_image(image), m_image_to_subtract(
          std::make_shared<ConstantImage<T>>(image->getWidth(), image->getHeight(), value_to_subtract)) {};

  SubtractImage(std::shared_ptr<Image<T>> image, std::shared_ptr<Image<T>> image_to_subtract)
      : m_image(image), m_image_to_subtract(image_to_subtract) {
    assert(m_image->getWidth() == m_image_to_subtract->getWidth());
    assert(m_image->getHeight() == m_image_to_subtract->getHeight());
  };

  using Image<T>::getValue;
  T getValue(int x, int y) const override {
    return m_image->getValue(x, y) - m_image_to_subtract->getValue(x, y);
  }
  
  int getWidth() const override {
    return m_image->getWidth();
  }
  
  int getHeight() const override {
    return m_image->getHeight();
  }

private:
  std::shared_ptr<Image<T>> m_image, m_image_to_subtract;

}; /* End of SubtractImage class */

} /* namespace SExtractor */


#endif
